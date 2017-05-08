/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include <vector>
#include <cstdint>

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

		RaftServer(int32_t id, const std::vector<RpcConnection *> &peers);

		~RaftServer();

		bool Put(const Log &log);

		void Vote(const RequestVoteArgs *args, RequestVoteReply *reply);

		void AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply);

		void Close();

		void Print() const;

	private:
		void Background();

		void RunElection();

		void RequestVote();

		void SendAppendEntry();

		int32_t  id_;

		uint8_t  state_;
		bool     running_;
		bool     in_election_;
		bool     election_time_out_;
		bool     reset_timer_;

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

		std::vector<int32_t> next_;
		std::vector<int32_t> match_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */