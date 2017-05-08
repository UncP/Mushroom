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
uint32_t RaftServer::HeartbeatInterval = 30;

RaftServer::RaftServer(uint32_t id, const std::vector<RpcClient *> &peers)
:id_(id), state_(Follower), running_(true), in_election_(false), election_time_out_(false),
reset_timer_(false), term_(0), vote_for_(-1), commit_(-1), applied_(-1), peers_(peers)
{
	background_thread_ = new Thread([this]() { this->Background(); });
	background_thread_->Start();
	eletion_thread_    = new Thread([this]() { this->RunElection(); });
	election_thread_->Start();
}

void RaftServer::Close()
{
	if (!running_)
		return ;

	mutex_.Lock();
	in_election_ = false;
	running_     = false;
	mutex_.Unlock();

	background_cond_.Signal();
	background_thread_->Stop();
	election_cond_.Signal();
	election_thread_->Stop();
}

RaftServer::~RaftServer()
{
	Close();
	delete background_thread_;
	delete election_thread_;
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
		if (state_ != Follower) {
			in_election_ = false;
			mutex_.Unlock();
			continue;
		}
		state_ = Candidate;
		++term_;
		vote_for_ = id_;
		RequestVoteArgs args(term_, id_, commit_, commit_ >= 0 ? logs_[commit_].term_ : 0);
		for (auto e : peers_) {
			e->Call("RaftServer::Vote", args);
		}
	}
}

void RaftServer::SendAppendEntry()
{
	for (;;) {
		mutex_.Lock();
		while (!in_election_ && running_)
			eletion_cond_.Wait()
		if (state_ != Leader) {
			mutex_.Unlock();
			break;
		}

		usleep(HeartbeatInterval * 1000);
	}
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
			reset_timer_       = false;
			election_time_out_ = false;
			mutex_.Unlock();
			continue;
		}
		if (state_ == Follower) {
			in_election_ = true;
			mutex_.Unlock();
			election_cond_.Signal();
		} else if (state_ == Leader) {
			mutex_.Unlock();
			SendAppendEntry();
		}
	}
}

} // namespace Mushroom
