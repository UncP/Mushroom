/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include <cstdint>

namespace Mushroom {

class RaftServer
{
	public:
		enum State { Follower, Candidate, Leader };

		RaftServer(uint32_t heartbeat_interval, uint32_t election_interval);

		~RaftServer();

		void Run();

	private:
		State    state_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		uint32_t heartbeat_interval_;
		uint32_t election_interval_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */