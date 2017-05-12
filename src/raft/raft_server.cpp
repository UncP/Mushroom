/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <ctime>
#include <random>
#include <unistd.h>

#include "raft_server.hpp"
#include "../include/thread.hpp"
#include "log.hpp"
#include "arg.hpp"

namespace Mushroom {

uint32_t RaftServer::ElectionTimeoutBase  = 150;
uint32_t RaftServer::ElectionTimeoutLimit = 300;
uint32_t RaftServer::HeartbeatInterval    = 30;

RaftServer::RaftServer(int32_t id, const std::vector<RpcConnection *> &peers)
:id_(id), state_(Follower), running_(true), in_election_(false), election_time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), peers_(peers)
{
	next_ .resize(peers_.size());
	match_.resize(peers_.size());

	background_thread_ = new Thread([this]() { this->Background(); });
	background_thread_->Start();

	election_thread_   = new Thread([this]() { this->RunElection(); });
	election_thread_->Start();
}

void RaftServer::Close()
{
	if (!running_)
		return ;

	mutex_.Lock();
	running_     = false;
	mutex_.Unlock();

	background_cond_.Signal();
	background_thread_->Stop();
	election_cond_.Signal();
	election_thread_->Stop();

	for (auto e : peers_)
		e->Close();
}

RaftServer::~RaftServer()
{
	Close();
	delete background_thread_;
	delete election_thread_;
}

bool RaftServer::Put(const Log &log)
{
	mutex_.Lock();
	if (state_ != Leader) {
		mutex_.Unlock();
		return false;
	}
	logs_.push_back(log);
	mutex_.Unlock();
	return true;
}

void RaftServer::Vote(const RequestVoteArgs *args, RequestVoteReply *reply)
{
	mutex_.Lock();
	*reply = RequestVoteReply(term_, false);
	if (args->term_ < term_)
		goto end;
	if (vote_for_ != -1 && vote_for_ != args->id_)
		goto end;
	if (args->last_index_ < commit_)
		goto end;
	if (commit_ >= 0 && args->last_term_ < logs_[commit_].term_)
		goto end;

	state_ = Follower;
	vote_for_ = args->id_;
	reset_timer_ = true;
	reply->granted_ = true;

end:
	mutex_.Unlock();
}

void RaftServer::AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply)
{
	mutex_.Lock();
	*reply = AppendEntryReply(term_, -1);
	if (args->term_ < term_)
		goto end;
	if (args->prev_index_ >= int32_t(logs_.size()))
		goto end;
	if (args->prev_index_ >= 0 && logs_[args->prev_index_].term_ != args->prev_term_)
		goto end;

	reply->success_ = 0;
	if (args->entries_.size()) {
		int32_t start = 0, prev_index = args->prev_index_;
		if (prev_index >= 0) {
			// find index in entries that does not overlap with current raft server
			for (int32_t i = int32_t(args->entries_.size()) - 1; i >= 0; --i) {
				if (logs_[prev_index].number_ == args->entries_[i].number_) {
					start = i + 1;
					break;
				}
			}
			// erase any entry that has same log but different term
			for (int32_t i = start - 1; i >= 0 && prev_index >= 0; --i) {
				if (logs_[prev_index].term_ != args->entries_[i].term_)
					--prev_index;
			}
		}
		logs_.erase(logs_.begin() + prev_index + 1, logs_.end());
		// prev_index should be exactly logs_.size()-1
		if ((args->entries_.begin() + start) < args->entries_.end()) {
			logs_.insert(logs_.end(), args->entries_.begin() + start, args->entries_.end());
			reply->success_ = args->entries_.end() - (args->entries_.begin() + start);
		}
	}

	if (args->leader_commit_ > commit_)
		commit_ = std::min(args->leader_commit_, int32_t(logs_.size()) - 1);
	reset_timer_ = true;
	background_cond_.Signal();

end:
	mutex_.Unlock();
}

void RaftServer::RunElection()
{
	for (;;) {
		mutex_.Lock();
		while (!in_election_ && running_)
			election_cond_.Wait(mutex_);
		if (!running_) {
			mutex_.Unlock();
			break;
		}
		assert(state_ == Follower);
		in_election_ = false;
		state_ = Candidate;
		++term_;
		vote_for_ = id_;
		RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
		for (auto e : peers_) {
			// e->Call("RaftServer::Vote", args);
		}
	}
}

void RaftServer::SendAppendEntry()
{
	Future *futures[peers_.size()];
	AppendEntryArgs args[peers_.size()];
	AppendEntryReply replys[peers_.size()];
	uint32_t future_size = 0;
	for (size_t i = 0;  && i < peers_.size(); ++i) {
		do {
			rf.mu.Lock();
			if (state_ != Leader)
			args[i].term_ = term_;
			args[i].id_ = 0;
			args[i].prev_index_ = next_[i] - 1;
			args[i].prev_term_  = args[i].prev_index_ >= 0 ? logs_[prev].term_ : 0;
			rf.mu.Unlock();

			peers_[i].Call("Raft::AppendEntry", &args, &reply);
		} while (state_.get() == Leader);
	}
	if (state_.get())
	for (uint32_t i = 0; i < future_size; ++i)
		futures[i]->Abandon();
}

void RaftServer::Background()
{
	std::default_random_engine engine(time(0));
	std::uniform_int_distribution<int> dist(ElectionTimeoutBase, ElectionTimeoutLimit);
	for (;;) {
		mutex_.Lock();
		while (!election_time_out_ && !reset_timer_ && running_)
			election_time_out_ = background_cond_.TimedWait(mutex_, dist(engine));
		if (!running_)
			break;
		if (reset_timer_) {
			reset_timer_ = false;
			mutex_.Unlock();
			continue;
		}
		if (in_election_) {
		}
		election_time_out_ = false;
		if (state_ == Follower) {
			in_election_ = true;
			mutex_.Unlock();
			election_cond_.Signal();
		} else {
			mutex_.Unlock();
			while (SendAppendEntry())
				usleep(50 * 1000);
	}
}

void RaftServer::Print() const
{

}

} // namespace Mushroom
