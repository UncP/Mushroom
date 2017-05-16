/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <random>
#include <unistd.h>

#include "raft_server.hpp"
#include "../include/bounded_queue.hpp"
#include "../network/eventbase.hpp"
#include "log.hpp"
#include "arg.hpp"

namespace Mushroom {

uint32_t RaftServer::ElectionTimeout   = 1000;
uint32_t RaftServer::HeartbeatInterval = 15;

RaftServer::RaftServer(int32_t id, const std::vector<RpcConnection *> &peers)
:id_(id), state_(Follower), running_(true), in_election_(false), election_time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), peers_(peers)
{
	next_ .resize(peers_.size());
	match_.resize(peers_.size());

	queue_.
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
	delete thread_;
}

void RaftServer::BecomeFollower()
{
	state_    = Follower;
	vote_for_ = -1;
}

void RaftServer::BecomeCandidate()
{
	++term_;
	state_    = Candidate;
	vote_for_ = id_;
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
		cond_.Signal();
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
	cond_.Signal();
}

void RaftServer::Election()
{
	mutex_.Lock();
	BecomeCandidate();
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
	sleep:
		while (running_ && !reset_timer_)
			cond_.TimedWait(mutex_, TimeUtil::GetTimeOut());
		if (!running_) {
			mutex_.Unlock();
			return ;
		}
		if (reset_timer_) {
			reset_timer_ = false;
			mutex_.Unlock();
			continue;
		}
		for (;;) {
			state_ = Candidate;
			++term_;
			vote_for_ = id_;
			RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
			mutex_.Unlock();
			Election(&args);
			if (status == TimeOut) {
				mutex_.Lock();
			} else {
				if (status == Success) {
					while (SendAppendEntry())
						TimeUtil::SleepFor(HeartbeatInterval);
				}
				mutex_.Lock();
				state_    = Follower;
				vote_for_ = -1;
				goto sleep;
			}
		}
	}
}

} // namespace Mushroom
