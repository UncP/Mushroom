/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:21:08
**/

#include <cassert>
#include <random>

#include "raft_server.hpp"
#include "../utility/thread.hpp"

namespace Mushroom {

static inline void* run(void *raft_server)
{
	((RaftServer *)raft_server)->Run();
	return 0;
}

uint32_t RaftServer::ElectionInterval = 150;

uint32_t RaftServer::HeartbeatInterval = 30;

RaftServer::RaftServer()
:running_(true), state_(Follower), term_(0), vote_for_(-1), commit_(-1), applied_(-1),
election_time_out_(false), reset_timer_(false),
election_thread_(new Thread<RaftServer>(&run, this))
{
	election_thread_->Start();
}

RaftServer::~RaftServer()
{
	delete election_thread_;
}

void RaftServer::Run()
{
	std::default_random_engine engine;
	std::uniform_int_distribution<uint32_t> distribution(0, 150);
	for (; running_;) {
		mutex_.Lock();
		while (!election_time_out_ && !reset_timer_)
			election_time_out_ = election_cond_.TimedWait(mutex_, distribution(engine));
		if (reset_timer_) {
			reset_timer_ = false;
			election_time_out_ = false;
			mutex_.Unlock();
			continue;
		}
	}
}

} // namespace Mushroom
