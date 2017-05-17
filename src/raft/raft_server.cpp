/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <random>
#include <unistd.h>
#include <cassert>

#include "raft_server.hpp"
#include "../include/bounded_queue.hpp"
#include "../network/eventbase.hpp"
#include "log.hpp"
#include "arg.hpp"

namespace Mushroom {

uint32_t RaftServer::ElectionTimeout   = 1000;
uint32_t RaftServer::HeartbeatInterval = 15;

RaftServer::RaftServer(int32_t id, const std::vector<RpcConnection *> &peers)
:id_(id), state_(Follower), running_(true), time_out_(false), election_time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), peers_(peers)
{
	next_ .resize(peers_.size());
	match_.resize(peers_.size());

	queue_ = new BoundedQueue<Task>(8);

	heartbeat_id_ = new TimerId();
}

void RaftServer::Close()
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return ;
	}

	running_ = false;
	mutex_.Unlock();

	cond_.Signal();
	thread_->Stop();

	for (auto e : peers_)
		e->Close();
}

RaftServer::~RaftServer()
{
	Close();
	delete queue_;
	delete heartbeat_id_;
}

void RaftServer::BecomeFollower()
{
	if (state_ == Leader)
		event_base_->Cancel(*heartbeat_id_);
	state_    = Follower;
	vote_for_ = -1;
}

void RaftServer::BecomeCandidate()
{
	++term_;
	state_    = Candidate;
	vote_for_ = id_;
}

void RaftServer::BecomeLeader()
{
	state_    = Leader;
	vote_for_ = -1;
	for (auto &e : next_)
		e = commit_ + 1;
	for (auto &e : match_)
		e = -1;
}

void RaftServer::Vote(const RequestVoteArgs *args, RequestVoteReply *reply)
{
	mutex_.Lock();
	*reply = {term_, false};
	const RequestVoteArgs &arg = *args;
	if (arg.term_ < term_)
		goto end;

	if (arg.term_ > term_) {
		BecomeFollower();
		term_ = arg.term_;
	}

	if (vote_for_ != -1 && vote_for_ != arg.id_)
		goto end;
	if (arg.last_index_ < commit_)
		goto end;
	if (commit_ >= 0 && arg.last_term_ != logs_[commit_].term_)
		goto end;

	reply->granted_ = true;

	vote_for_ = arg.id_;

	reset_timer_ = true;

end:
	mutex_.Unlock();
	if (reply->granted_)
		back_cond_.Signal();
}

void RaftServer::AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply)
{
	mutex_.Lock();
	*reply = {term_, -1};
	const AppendEntryArgs &arg = *args;
	if (arg.term_ < term_)
		goto end;

	if (arg.term_ >= term_) {
		BecomeFollower();
		term_ = arg.term_;
	}

	int32_t  prev_i = arg.prev_index_;
	uint32_t prev_t = arg.prev_term_;
	if (prev_i >= int32_t(logs_.size()))
		goto end;
	if (prev_i >= 0 && logs_[prev_i].term_ != prev_t)
		goto end;

	if (arg.entries_.empty() && ++prev_i < logs_.size())
		logs_.erase(logs_.begin() + prev_i, logs_.end());
	uint32_t prev_j = 0;
	for (; prev_i < logs_.size() && prev_j < arg.entries_.size(); ++prev_i, ++prev_j) {
		if (logs_[prev_i].term_ != arg.entries_[prev_j].term_) {
			logs_.erase(logs_.begin() + prev_i, logs_.end());
			break;
		}
	}
	assert(prev_i == logs_.size());
	logs_.insert(logs_.end(), arg.entries_.begin() + prev_j, arg.entries_.end());
	reply->success_ = arg.entries_.end() - (arg.entries_.begin() + prev_j);

	if (args->leader_commit_ > commit_)
		commit_ = std::min(args->leader_commit_, int32_t(logs_.size()) - 1);

end:
	reset_timer_ = true;
	mutex_.Unlock();
	back_cond_.Signal();
}

void RaftServer::Election()
{
	mutex_.Lock();
	if (state_ == Follower) {
		BecomeCandidate();
	} else {
		mutex_.Unlock()
		return ;
	}
	RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
	mutex_.Unlock();

	TimerId id = event_base_->RunAfter(ElectionTimeout, [this]() {
		mutex_.Lock();
		if (state_ == Candidate) {
			mutex_.Unlock();
			Election();
		} else {
			mutex_.Unlock();
		}
	});

	uint32_t size = peers.size();
	RequestVoteReply replys[size];
	Future *futures[size];
	for (uint32_t i = 0; i < size; ++i) {
		queue_->Push([this, i, futures, args, &replys]() {
			peers_[i]->Call("RaftServer::Vote", args, &replys[i]);
		});
	}

	TimeUtil::SleepFor(left);
	uint32_t vote = 1;
	for (uint32_t i = 0; i < size; ++i) {
		if (futures[i]->ok()) {
			if (replys[i].granted_)
				++vote;
			else if (term_ < replys[i].term_)
				return Fail;
		} else {
			futures[i].Abandon();
		}
	}
	if (vote > ((size + 1) / 2))
		return Success;
	return TimeOut;
}

bool RaftServer::SendAppendEntry()
{
	// uint32_t size = peers_.size();
	// Future *futures[size];
	// AppendEntryArgs args[size];
	// AppendEntryReply replys[size];
	// mutex_.Lock();
	// for (size_t i = 0; i < peers_.size(); ++i) {
	// 	int32_t prev = next_[i] - 1;
	// 	args[i] = AppendEntryArgs(term_, id_, prev, prev >= 0 ? logs_[prev].term_ : 0, commit_);
	// 	if (prev >= 0) {
	// 		args[i].entries_.reserve(logs_.size() - prev);
	// 		for (uint32_t j = prev; j < logs_.size(); ++j)
	// 			args[i].entries_.push_back(logs_[j]);
	// 	}
	// 	queue_->Push([this, i, &args, &replys]() {
	// 		peers_[i].Call("RaftServer::AppendEntry", &args[i], &replys[i]);
	// 	});
	// }
	// mutex_.Unlock();
	// if (state_.get())
	// for (uint32_t i = 0; i < future_size; ++i)
	// 	futures[i]->Abandon();
}

void RaftServer::Background()
{
	for (;;) {
		mutex_.Lock();
		while (running_ && !reset_timer_ && !time_out_ && state_ != Leader)
			time_out_ = back_cond_.TimedWait(mutex_, GetTimeOut());
		if (!running_) {
			mutex_.Unlock();
			break;
		}
		time_out_ = false;
		if (state_ == Leader) {
			mutex_.Unlock();
			continue;
		}
		if (reset_timer_) {
			reset_timer_ = false;
			mutex_.Unlock();
			continue;
		}
		for (;;) {
			mutex_.Unlock();
			event_base_->RunNow([this]() { Election(); })
			mutex_.Lock();
			while (running_ && !election_time_out_)
				election_time_out_ = election_cond_.TimedWait(mutex_, ElectionTimeout);
			if (!running_) {
				mutex_.Unlock();
				break;
			}
			if (state_ == Candidate && election_time_out_) {
				election_time_out_ = false;
				continue;
			} else if (state_ == Candidate) {
				BecomeLeader();
				mutex_.Unlock();
				event_base_->RunEvery(HeartbeatInterval, [this] {
					SendAppendEntry();
				});
				break;
			} else {

			}
		}
	}
}

} // namespace Mushroom
