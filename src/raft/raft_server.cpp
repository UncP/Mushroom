/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <unistd.h>
#include <random>
#include <cassert>
#include <map>

#include "raft_server.hpp"
#include "../rpc/future.hpp"
#include "../rpc/rpc_connection.hpp"
#include "log.hpp"
#include "arg.hpp"
#include "../include/thread.hpp"

namespace Mushroom {

uint32_t RaftServer::TimeoutBase   = 150;
uint32_t RaftServer::TimeoutTop    = 300;
uint32_t RaftServer::ElectionTimeout   = 1000;
uint32_t RaftServer::HeartbeatInterval = 15;
uint32_t RaftServer::CommitInterval = 150;

RaftServer::RaftServer(EventBase *event_base, uint16_t port, int32_t id)
:RpcServer(event_base, port), id_(id), state_(Follower), running_(false), time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), futures_(0),
back_thread_(0)
{
	RpcServer::Start();
	Register("RaftServer::Vote", this, &RaftServer::Vote);
	Register("RaftServer::AppendEntry", this, &RaftServer::AppendEntry);
}

RaftServer::~RaftServer()
{
	Close();
	for (auto e : peers_)
		delete e;
	if (futures_)
		delete futures_;
	delete back_thread_;
}

void RaftServer::Start()
{
	running_ = true;
	back_thread_ = new Thread([this]() {
		Background();
	});
	back_thread_->Start();
}

void RaftServer::Close()
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return ;
	}

	Info("closing raft server %d", id_);

	RpcServer::Close();

	running_ = false;

	if (state_ == Follower) {
		back_cond_.Signal();
	} else if (state_ == Leader) {
		event_base_->Cancel(commit_id_);
		heartbeat_cond_.Signal();
	}

	mutex_.Unlock();
	back_thread_->Stop();
}

bool RaftServer::IsLeader()
{
	mutex_.Lock();
	bool ret = (state_ == Leader);
	mutex_.Unlock();
	return ret;
}

int32_t RaftServer::Id()
{
	return id_;
}

uint32_t RaftServer::Term()
{
	mutex_.Lock();
	uint32_t ret = term_;
	mutex_.Unlock();
	return ret;
}

void RaftServer::AddPeer(RpcConnection *peer)
{
	mutex_.Lock();
	peers_.push_back(peer);
	next_.push_back(0);
	match_.push_back(0);
	mutex_.Unlock();
}

int64_t RaftServer::GetTimeOut()
{
	static std::default_random_engine engine(time(0));
	static std::uniform_int_distribution<int64_t> distribution(TimeoutBase, TimeoutTop);
	return distribution(engine);
}

void RaftServer::BecomeFollower()
{
	Info("%d becoming follower, term %u", id_, term_);
	if (state_ == Leader) {
		event_base_->Cancel(commit_id_);
		heartbeat_cond_.Signal();
	}
	reset_timer_ = false;
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
	Info("%d becoming leader, term %u", id_, term_);
	state_    = Leader;
	for (auto &e : next_)
		e = commit_ + 1;
	for (auto &e : match_)
		e = -1;
	commit_id_ = event_base_->RunEvery(CommitInterval, [this]() {
		UpdateCommitIndex();
	});
}

void RaftServer::Vote(const RequestVoteArgs *args, RequestVoteReply *reply)
{
	mutex_.Lock();
	reply->granted_ = false;
	const RequestVoteArgs &arg = *args;
	bool sig = (state_ == Follower);
	if (arg.term_ < term_)
		goto end;

	if (arg.term_ > term_ && state_ != Follower)
		BecomeFollower();
	term_ = arg.term_;

	if (vote_for_ != -1 && vote_for_ != arg.id_)
		goto end;
	if (arg.last_index_ < commit_)
		goto end;
	if (commit_ >= 0 && arg.last_term_ != logs_[commit_].term_)
		goto end;

	Info("%d vote for %d", id_, arg.id_);

	reply->granted_ = true;

	vote_for_ = arg.id_;

	if (sig) {
		reset_timer_ = true;
		back_cond_.Signal();
	}

end:
	reply->term_ = term_;
	mutex_.Unlock();
}

void RaftServer::AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply)
{
	mutex_.Lock();
	reply->success_ = -1;
	const AppendEntryArgs &arg = *args;
	int32_t  prev_i = arg.prev_index_;
	uint32_t prev_t = arg.prev_term_;
	uint32_t prev_j = 0;

	if (state_ == Follower) {
		reset_timer_ = true;
		back_cond_.Signal();
	}

	if (arg.term_ < term_)
		goto end;

	if (arg.term_ > term_ && state_ != Follower)
		BecomeFollower();
	term_ = arg.term_;

	if (prev_i >= int32_t(logs_.size()))
		goto end;
	if (prev_i >= 0 && logs_[prev_i].term_ != prev_t)
		goto end;

	if (++prev_i < int32_t(logs_.size()) && arg.entries_.empty())
		logs_.erase(logs_.begin() + prev_i, logs_.end());
	for (; prev_i < int32_t(logs_.size()) && prev_j < arg.entries_.size(); ++prev_i, ++prev_j) {
		if (logs_[prev_i].term_ != arg.entries_[prev_j].term_) {
			logs_.erase(logs_.begin() + prev_i, logs_.end());
			break;
		}
	}
	assert(prev_i == int32_t(logs_.size()));
	logs_.insert(logs_.end(), arg.entries_.begin() + prev_j, arg.entries_.end());
	reply->success_ = arg.entries_.end() - (arg.entries_.begin() + prev_j);

	if (arg.leader_commit_ > commit_)
		commit_ = std::min(arg.leader_commit_, int32_t(logs_.size()) - 1);

end:
	reply->term_ = term_;
	mutex_.Unlock();
}

ElectionResult RaftServer::Election()
{
	if (!running_) {
		mutex_.Unlock();
		return Fail;
	}
	BecomeCandidate();
	RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
	Info("election: term %u id %d commit %d last %u", args.term_, args.id_, args.last_index_,
		args.last_term_);

	uint32_t size = peers_.size();
	RequestVoteReply replys[size];
	if (!futures_)
		futures_ = new FutureGroup(size);
	else
		futures_->Clear();
	for (uint32_t i = 0; i < size; ++i)
		futures_->Add(peers_[i]->Call("RaftServer::Vote", &args, &replys[i]));
	mutex_.Unlock();
	TimerId id = event_base_->RunAfter(ElectionTimeout, [this]() {
		futures_->Cancel();
	});

	futures_->Wait();
	event_base_->Cancel(id);
	mutex_.Lock();
	if (!running_ || state_ == Follower) {
		mutex_.Unlock();
		return Fail;
	}
	uint32_t vote = 1, timeout = 0;
	for (uint32_t i = 0; i < size; ++i) {
		if (!(*futures_)[i]->ok()) {
			++timeout;
			continue;
		}
		if (replys[i].granted_) {
			++vote;
		} else if (term_ < replys[i].term_) {
			BecomeFollower();
			term_ = replys[i].term_;
			mutex_.Unlock();
			return Fail;
		}
	}
	if (vote > ((size + 1) / 2)) {
		BecomeLeader();
		mutex_.Unlock();
		return Success;
	}
	if (timeout) {
		mutex_.Unlock();
		return Timeout;
	}
	BecomeFollower();
	mutex_.Unlock();
	return Fail;
}

void RaftServer::UpdateCommitIndex()
{
	mutex_.Lock();
	if (state_ != Leader) {
		mutex_.Unlock();
		return ;
	}
	std::map<int32_t, uint32_t> map;
	for (auto e : match_) {
		if (e < 0) continue;
		if (map.find(e) == map.end())
			map[e] = 2;
		else
			++map[e];
	}
	for (auto &e : map)
		if (e.second > ((peers_.size() + 1) / 2) && term_ == logs_[e.first].term_
			&& e.first > commit_) {
			commit_ = e.first;
			break;
		}
	mutex_.Unlock();
}

void RaftServer::SendAppendEntry()
{
	uint32_t size = peers_.size();
	AppendEntryArgs args[size];
	AppendEntryReply replys[size];
	mutex_.Lock();
	if (!running_ || state_ != Leader) {
		mutex_.Unlock();
		return ;
	}
	FutureGroup *futures = new FutureGroup(size);
	for (size_t i = 0; i < peers_.size(); ++i) {
		int32_t prev = next_[i] - 1;
		args[i] = {term_, id_, prev >= 0 ? logs_[prev].term_ : 0, prev, commit_};
		if (next_[i] < int32_t(logs_.size()))
			args[i].entries_.insert(args[i].entries_.end(), logs_.begin() + next_[i], logs_.end());
		futures->Add(peers_[i]->Call("RaftServer::AppendEntry", &args[i], &replys[i]));
	}
	mutex_.Unlock();
	TimerId id = event_base_->RunAfter(150, [futures]() {
		futures->Cancel();
	});

	futures->Wait();
	event_base_->Cancel(id);
	mutex_.Lock();
	if (!running_ || state_ != Leader) {
		mutex_.Unlock();
		delete futures;
		return ;
	}
	for (uint32_t i = 0; i < size; ++i) {
		if (!(*futures)[i]->ok()) continue;
		if (term_ < replys[i].term_) {
			BecomeFollower();
			term_ = replys[i].term_;
			mutex_.Unlock();
			delete futures;
			return ;
		} else if (term_ == replys[i].term_) {
			if (replys[i].success_ == -1) {
				if (next_[i] > 0)
					--next_[i];
			} else {
				next_[i] += replys[i].success_;
				match_[i] = next_[i] - 1;
			}
		}
	}
	mutex_.Unlock();
	delete futures;
}

void RaftServer::Background()
{
	for (;;) {
		mutex_.Lock();
		while (running_ && !reset_timer_ && !time_out_)
			time_out_ = back_cond_.TimedWait(mutex_, GetTimeOut());
		if (!running_) {
			mutex_.Unlock();
			return ;
		}
		time_out_ = false;
		if (reset_timer_) {
			reset_timer_ = false;
			mutex_.Unlock();
			continue;
		}
		ElectionResult r;
		for (; (r = Election()) == Timeout;)
			mutex_.Lock();
		for (; r == Success;) {
			bool time_out = false;
			mutex_.Lock();
			while (running_ && state_ == Leader && !time_out)
				time_out = heartbeat_cond_.TimedWait(mutex_, HeartbeatInterval);
			if (!running_) {
				mutex_.Unlock();
				return ;
			}
			mutex_.Unlock();
			if (time_out) {
				time_out = false;
				event_base_->RunNow([this]() { SendAppendEntry(); });
			} else {
				break;
			}
		}
	}
}

void RaftServer::Status()
{
	mutex_.Lock();
	Info("\033[31mid: %d\033[0m  state: \033[34m%s\033[0m  \033[33mterm: %u\033[0m  "
		"\033[32mcommit: %d\033[0m",
		id_, (state_ != Follower) ? (state_ == Leader ? "Leader" : "Candidate") : "Follower",
		term_, commit_);
	mutex_.Unlock();
}

} // namespace Mushroom
