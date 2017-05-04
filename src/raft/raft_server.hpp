/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include "../utility/latch.hpp"

namespace Mushroom {

template<typename T> class Thread;

class RaftServer
{
	public:
		enum State { Follower, Candidate, Leader };

		static uint32_t ElectionTimeoutBase;
		static uint32_t ElectionTimeoutLimit;
		static uint32_t HeartbeatInterval;

		RaftServer();

		~RaftServer();

		void Run();

		void Elect();

	private:
		int32_t  id_;

		bool     running_;
		State    state_;

		std::vector<RpcConnection *> peers_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		Mutex    mutex_;

		bool                election_time_out_;
		bool                reset_timer_;
		Thread<RaftServer> *election_thread_;
		ConditionVariable   election_cond_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */