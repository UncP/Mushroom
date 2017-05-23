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

namespace Mushroom {

uint32_t RaftServer::TimeoutBase   = 150;
uint32_t RaftServer::TimeoutTop    = 300;
uint32_t RaftServer::ElectionTimeout   = 1000;
uint32_t RaftServer::HeartbeatInterval = 15;

RaftServer::RaftServer(EventBase *event_base, uint16_t port, int32_t id)
:RpcServer(event_base, port), id_(id), state_(Follower), running_(true), term_(0),
vote_for_(-1), commit_(-1), applied_(-1)
{
	RpcServer::Start();
	Register("RaftServer::Vote", this, &RaftServer::Vote);
	Register("RaftServer::AppendEntry", this, &RaftServer::AppendEntry);
	RescheduleElection();
}

RaftServer::~RaftServer()
{
	for (auto e : peers_)
		delete e;
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
	if (state_ == Follower)
		event_base_->Cancel(election_id_);
	else if (state_ == Leader)
		event_base_->Cancel(heartbeat_id_);
	mutex_.Unlock();
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

void RaftServer::RescheduleElection()
{
	static std::default_random_engine engine(time(0));
	static std::uniform_int_distribution<int64_t> dist(TimeoutBase, TimeoutTop);
	event_base_->RescheduleAfter(&election_id_, dist(engine), [this]() {
		Election();
	});
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

void RaftServer::BecomeFollower(uint32_t term)
{
	Info("%d becoming follower, term %u", id_, term);
	if (state_ == Leader)
		event_base_->Cancel(heartbeat_id_);
	state_ = Follower;
	term_  = term;
	RescheduleElection();
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
	heartbeat_id_ = event_base_->RunEvery(HeartbeatInterval, [this]() {
		SendAppendEntry();
	});
}

void RaftServer::Vote(const RequestVoteArgs *args, RequestVoteReply *reply)
{
	mutex_.Lock();
	reply->granted_ = 0;
	const RequestVoteArgs &arg = *args;
	int32_t  curr_idx  = logs_.size() - 1;
	uint32_t last_term = (curr_idx >= 0) ? logs_[curr_idx].term_ : 0;
	uint32_t prev_term = term_;
	if (arg.term_ < term_)
		goto end;

	if (arg.term_ > term_)
		BecomeFollower(arg.term_);

	if (vote_for_ != -1 && vote_for_ != arg.id_)
		goto end;
	if (arg.last_term_ < last_term)
		goto end;
	if (arg.last_term_ == last_term && curr_idx > arg.last_index_)
		goto end;

	Info("%d vote for %d", id_, arg.id_);

	reply->granted_ = 1;
	vote_for_ = arg.id_;

	if (prev_term == term_)
		RescheduleElection();

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
	if ((arg.term_ > term_) || (arg.term_ == term_ && state_ == Candidate))
		BecomeFollower(arg.term_);
	else
		RescheduleElection(); // what if (arg.term_ == term_ && state_ == Leader)
	if (arg.term_ < term_)
		goto end;

	if (prev_i >= int32_t(logs_.size()))
		goto end;
	if (prev_i >= 0 && logs_[prev_i].term_ != prev_t) {
		logs_.erase(logs_.begin() + prev_i, logs_.end());
		goto end;
	}

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
	reply->curr_idx_ = logs_.size() - 1;
	reply->term_ = term_;
	mutex_.Unlock();
}

void RaftServer::Election()
{
	mutex_.Lock();
	if (!running_ || state_ != Follower) {
		mutex_.Unlock();
		return ;
	}
	BecomeCandidate();
	RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
	mutex_.Unlock();

	Info("election: term %u id %d commit %d last %u", args.term_, args.id_, args.last_index_,
		args.last_term_);

	uint32_t size = peers_.size();
	Future<RequestVoteReply> futures[size];
	TimerId id = event_base_->RunAfter(ElectionTimeout, [&futures, size]() {
		for (uint32_t i = 0; i < size; ++i)
			futures[i].Cancel();
	});
	for (uint32_t i = 0; i < size; ++i)
		peers_[i]->Call("RaftServer::Vote", &args, &futures[i]);
	for (uint32_t i = 0; i < size; ++i)
		futures[i].Wait();
	event_base_->Cancel(id);

	mutex_.Lock();
	if (!running_ || state_ == Follower) {
		mutex_.Unlock();
		return ;
	}
	uint32_t vote = 1, timeout = 0;
	for (uint32_t i = 0; i < size; ++i) {
		if (!futures[i].ok()) {
			++timeout;
			continue;
		}
		RequestVoteReply &reply = futures[i].Value();
		if (reply.granted_) {
			++vote;
		} else if (term_ < reply.term_) {
			BecomeFollower(reply.term_);
			mutex_.Unlock();
			return ;
		}
	}
	if (vote > ((size + 1) / 2)) {
		BecomeLeader();
		mutex_.Unlock();
		return ;
	}
	if (timeout) {
		mutex_.Unlock();
		event_base_->RunNow([this]() { Election(); });
		return ;
	}
	BecomeFollower(term_);
	mutex_.Unlock();
	return ;
}

void RaftServer::SendAppendEntry()
{
	mutex_.Lock();
	if (!running_ || state_ != Leader) {
		mutex_.Unlock();
		return ;
	}
	uint32_t size = peers_.size();
	AppendEntryArgs args[size];
	Future<AppendEntryReply> *futures = new Future<AppendEntryReply>[size];
	for (size_t i = 0; i < peers_.size(); ++i) {
		int32_t prev = next_[i] - 1;
		args[i] = {term_, id_, prev >= 0 ? logs_[prev].term_ : 0, prev, commit_};
		if (next_[i] < int32_t(logs_.size()))
			args[i].entries_.insert(args[i].entries_.end(), logs_.begin() + next_[i], logs_.end());
		Future<AppendEntryReply> *fu = futures + i;
		peers_[i]->Call("RaftServer::AppendEntry", &args[i], fu);
		fu->OnCallback([this, i, fu]() {
			const auto &reply = fu->Value();
			ReceiveAppendEntryReply(i, reply);
		});
	}
	mutex_.Unlock();
	event_base_->RunAfter(TimeoutBase, [this, futures, size]() {
		for (uint32_t i = 0; i != size; ++i) {
			peers_[i]->RemoveFuture(&futures[i]);
			futures[i].Cancel();
		}
		delete [] futures;
	});
}

void RaftServer::ReceiveAppendEntryReply(uint32_t i, const AppendEntryReply &reply)
{
	mutex_.Lock();
	uint32_t vote = 1;
	if (state_ != Leader)
		goto end;
	if (reply.term_ > term_) {
		BecomeFollower(reply.term_);
		goto end;
	}
	if (reply.term_ != term_ || reply.success_ == 0)
		goto end;
	if (reply.success_ == -1) {
		if (next_[i] > 0)
			--next_[i];
	} else {
		next_[i] += reply.success_;
		match_[i] = next_[i] - 1;
	}

	if (commit_ >= reply.curr_idx_ || logs_[reply.curr_idx_].term_ != term_)
		goto end;
	for (uint32_t i = 0; i < peers_.size(); ++i)
		if (match_[i] >= reply.curr_idx_)
			++vote;
	if (vote > ((peers_.size() + 1) / 2))
		commit_ = reply.curr_idx_;
end:
	mutex_.Unlock();
}

} // namespace Mushroom
