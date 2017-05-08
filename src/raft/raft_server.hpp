/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include <vector>

#include "../include/mutex.hpp"
#include "../include/cond.hpp"

namespace Mushroom {

class Log;
class Thread;
class RpcConnection;
class RequestVoteArgs;
class RequestVoteReply;
class AppendEntryArgs;
class AppendEntryReply;

class RaftServer
{
	public:
		enum State { Follower = 0x0, Candidate = 0x1, Leader = 0x2 };

		static uint32_t ElectionTimeoutBase;
		static uint32_t ElectionTimeoutLimit;
		static uint32_t HeartbeatInterval;

		RaftServer(uint32_t id, const std::vector<RpcConnection *> &peers);

		~RaftServer();

		void Close();

	private:
		void Background();

		void RunElection();

		void Vote(const RequestVoteArgs *args, RequestVoteReply *reply);

		void RequestVote();

		void AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply);

		void SendAppendEntry();

		uint32_t id_;

		uint8_t  state_;
		uint8_t  running_;
		uint8_t  in_election_;
		uint8_t  election_time_out_;
		uint8_t  reset_timer_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		std::vector<Log> logs_;

		Mutex    mutex_;

		Thread  *background_thread_;
		Cond     background_cond_;
		Thread  *election_thread_;
		Cond     election_cond_;

		std::vector<RpcConnection *> peers_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */