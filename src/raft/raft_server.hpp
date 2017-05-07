/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include "../utility/mutex.hpp"
#include "../utility/cond.hpp"

namespace Mushroom {

class Log;
class Thread;

class RaftServer
{
	public:
		enum State { Follower = 0x0, Candidate = 0x1, Leader = 0x2 };

		static uint32_t ElectionTimeoutBase;
		static uint32_t ElectionTimeoutLimit;
		static uint32_t HeartbeatInterval;

		RaftServer();

		~RaftServer();

	private:
		void Background();

		void RunElection();

		uint32_t  id_;
		uint8_t  running_;

		Mutex    mutex_;
		uint8_t  state_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		std::vector<Log *> logs_;

		uint8_t  election_time_out_;
		uint8_t  reset_timer_;
		Thread  *election_thread_;
		Cond     election_cond_;

		std::vector<RpcConnection *> peers_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */