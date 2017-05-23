/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:19:57
**/

#ifndef _RAFT_SERVER_HPP_
#define _RAFT_SERVER_HPP_

#include <vector>

#include "../include/utility.hpp"
#include "../include/mutex.hpp"
#include "../include/cond.hpp"
#include "../rpc/rpc_server.hpp"
#include "../network/eventbase.hpp"

namespace Mushroom {

class Log;
class RpcConnection;
class RequestVoteArgs;
class RequestVoteReply;
class AppendEntryArgs;
class AppendEntryReply;

class RaftServer : public RpcServer
{
	public:
		enum State { Follower = 0x0, Candidate, Leader };

		RaftServer(EventBase *event_base, uint16_t port, int32_t id);

		~RaftServer();

		bool IsLeader(uint32_t *term);

		int32_t Id();

		uint32_t Term();

		void Status();

		void Close();

		void AddPeer(RpcConnection *peer);

		std::vector<RpcConnection *>& Peers();

		void Vote(const RequestVoteArgs *args, RequestVoteReply *reply);

		void AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply);

		static uint32_t ElectionTimeout;

		void RescheduleElection();

	private:
		static uint32_t TimeoutBase;
		static uint32_t TimeoutTop;
		static uint32_t HeartbeatInterval;

		static int64_t GetElectionTimeout();

		void Election();

		void RequestVote();

		void SendAppendEntry();

		void BecomeFollower(uint32_t term);

		void BecomeCandidate();

		void BecomeLeader();

		void UpdateCommitIndex();

		void ReceiveAppendEntryReply(uint32_t i, const AppendEntryReply &reply);

		using RpcServer::Register;
		using RpcServer::event_base_;

		int32_t  id_;

		uint8_t  state_;
		bool     running_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		std::vector<Log> logs_;

		std::vector<int32_t> next_;
		std::vector<int32_t> match_;

		std::vector<RpcConnection *> peers_;

		Mutex mutex_;

		TimerId  election_id_;
		TimerId  heartbeat_id_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */