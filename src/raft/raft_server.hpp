/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-22 21:19:57
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

class MushroomLog;
class RpcConnection;
class RequestVoteArgs;
class RequestVoteReply;
class AppendEntryArgs;
class AppendEntryReply;

class RaftServer : public RpcServer
{
	public:
		enum State { Follower = 0x0, Candidate, Leader };

		typedef std::function<bool(const MushroomLog &log)> ApplyFunc;

		RaftServer(EventBase *event_base, uint16_t port, int32_t id);

		~RaftServer();

		bool IsLeader(uint32_t *term);

		int32_t Id();

		uint32_t Term();

		void Status(bool print_log = false, bool print_next = false);

		void SetApplyFunc(ApplyFunc &&func);

		void Start();

		bool Start(MushroomLog &log, uint32_t *index);

		bool LogAt(uint32_t index, MushroomLog &log);

		void Close();

		void AddPeer(RpcConnection *peer);

		std::vector<RpcConnection *>& Peers();

		void Vote(const RequestVoteArgs *args, RequestVoteReply *reply);

		void AppendEntry(const AppendEntryArgs *args, AppendEntryReply *reply);

		static uint32_t ElectionTimeoutBase;

	private:
		static uint32_t TimeoutBase;
		static uint32_t TimeoutTop;
		static uint32_t HeartbeatInterval;

		static int64_t GetElectionTimeout();

		void RescheduleElection();

		void SendRequestVote();

		void RequestVote();

		void SendAppendEntry();

		void BecomeFollower(uint32_t term);

		void BecomeCandidate();

		void BecomeLeader();

		void UpdateCommitIndex();

		void ReceiveRequestVoteReply(const RequestVoteReply &reply);

		void ReceiveAppendEntryReply(uint32_t i, const AppendEntryReply &reply);

		using RpcServer::Register;
		using RpcServer::event_base_;

		int32_t  id_;

		uint8_t  state_;
		uint8_t  running_;

		uint32_t term_;
		int32_t  vote_for_;
		int32_t  commit_;
		int32_t  applied_;

		uint32_t votes_;

		std::vector<MushroomLog> logs_;

		std::vector<int32_t> next_;
		std::vector<int32_t> match_;

		std::vector<RpcConnection *> peers_;

		Mutex mutex_;

		TimerId  election_id_;
		TimerId  heartbeat_id_;
		TimerId  timeout_id_;

		ApplyFunc apply_func_;
};

} // namespace Mushroom

#endif /* _RAFT_SERVER_HPP_ */