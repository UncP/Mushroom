/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <ctime>
#include <random>

#include "raft_server.hpp"
#include "../include/thread.hpp"
#include "log.hpp"

namespace Mushroom {

uint32_t RaftServer::ElectionTimeoutBase  = 150;
uint32_t RaftServer::ElectionTimeoutLimit = 300;
uint32_t RaftServer::HeartbeatInterval = 30;

RaftServer::RaftServer()
:running_(true), state_(Follower), term_(0), vote_for_(-1), commit_(-1), applied_(-1),
election_time_out_(false), reset_timer_(false)
{
	election_thread_ = new Thread([this]() { this->Background(); });
	election_thread_->Start();
}

RaftServer::Close()
{
	if (!running_)
		return ;

	running_ = false;

	election_cond_.Signal();
	election_thread_->Stop();
}

RaftServer::~RaftServer()
{
	Close();

	delete election_thread_;
}

void RaftServer::Election()
{
	mutex_.Lock();
	if (state_ != Follower) {
		mutex_.Unlock();
		return ;
	}
	for (auto e : peers_) {

	}
	mutex_.Unlock();
}

void RaftServer::Background()
{
	std::default_random_engine engine(time(0));
	std::uniform_int_distribution<int> dist(ElectionTimeoutBase, ElectionTimeoutLimit);
	for (;;) {
		mutex_.Lock();
		while (!election_time_out_ && !reset_timer_ && running_)
			election_time_out_ = election_cond_.TimedWait(mutex_, dist(engine));
		if (!running_)
			break;
		if (reset_timer_) {
			reset_timer_       = false;
			election_time_out_ = false;
			mutex_.Unlock();
			continue;
		}
		Election();
	}
}

} // namespace Mushroom
