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

enum ElectionResult { Success, Fail, Timeout };

class RaftServer : public RpcServer
{
	public:
		enum State { Follower = 0x0, Candidate, Leader };

		RaftServer(EventBase *event_base, uint16_t port, int32_t id);

		~RaftServer();

		bool IsLeader();

		int32_t Id();

		uint32_t Term();

		void Status();

		void Vote(const RequestVoteArgs *args, RequestVoteReply *reply);

		void AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply);

		void Close();

		void AddPeer(RpcConnection *peer);

		void Start();

	private:
		static uint32_t TimeoutBase;
		static uint32_t TimeoutTop;
		static uint32_t ElectionTimeout;
		static uint32_t HeartbeatInterval;
		static uint32_t CommitInterval;

		void Election();

		void RequestVote();

		void SendAppendEntry();

		void RescheduleElection();

		void BecomeFollower(uint32_t term);

		void BecomeCandidate();

		void BecomeLeader();

		void UpdateCommitIndex();

		using RpcServer::Register;
		using RpcServer::event_base_;

		int32_t  id_;

		uint8_t  state_;
		bool     running_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		TimerId  election_id_;
		TimerId  heartbeat_id_;
		TimerId  commit_id_;

		std::vector<Log> logs_;

		Mutex mutex_;

		std::vector<RpcConnection *> peers_;

		std::vector<int32_t> next_;
		std::vector<int32_t> match_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */