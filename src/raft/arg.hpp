/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-08 09:15:27
**/

#ifndef _RAFT_ARG_HPP_
#define _RAFT_ARG_HPP_

#include "../rpc/marshaller.hpp"
#include "log.hpp"

namespace Mushroom {

struct RequestVoteArgs
{
	RequestVoteArgs() { }
	RequestVoteArgs(uint32_t term, uint32_t id, uint32_t last_index, uint32_t last_term)
	:term_(term), id_(id), last_index_(last_index), last_term_(last_term) { }
	uint32_t  term_;
	int32_t   id_;
	int32_t   last_index_;
	uint32_t  last_term_;
};

struct RequestVoteReply
{
	RequestVoteReply() { }
	uint32_t term_;
	uint32_t granted_;
};

struct AppendEntryArgs
{
	AppendEntryArgs() { }

	AppendEntryArgs(uint32_t term, int32_t id, uint32_t prev_term, int32_t prev_index,
		int32_t leader_commit):term_(term), id_(id), prev_term_(prev_term),
	prev_index_(prev_index), leader_commit_(leader_commit) { }
	uint32_t         term_;
	int32_t          id_;
	uint32_t         prev_term_;
	int32_t          prev_index_;
	int32_t          leader_commit_;
	std::vector<Log> entries_;
};

struct AppendEntryReply
{
	AppendEntryReply() { }
	uint32_t term_;
	int32_t  success_;
	int32_t  curr_idx_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const RequestVoteArgs &args)
{
	marshaller << args.term_;
	marshaller << args.id_;
	marshaller << args.last_index_;
	marshaller << args.last_term_;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, RequestVoteArgs &args)
{
	marshaller >> args.term_;
	marshaller >> args.id_;
	marshaller >> args.last_index_;
	marshaller >> args.last_term_;
	return marshaller;
}

inline Marshaller& operator<<(Marshaller &marshaller, const RequestVoteReply &reply)
{
	marshaller << reply.term_;
	marshaller << reply.granted_;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, RequestVoteReply &reply)
{
	marshaller >> reply.term_;
	marshaller >> reply.granted_;
	return marshaller;
}

inline Marshaller& operator<<(Marshaller &marshaller, const AppendEntryArgs &args)
{
	marshaller << args.term_;
	marshaller << args.id_;
	marshaller << args.prev_term_;
	marshaller << args.prev_index_;
	marshaller << args.leader_commit_;
	marshaller << args.entries_;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, AppendEntryArgs &args)
{
	marshaller >> args.term_;
	marshaller >> args.id_;
	marshaller >> args.prev_term_;
	marshaller >> args.prev_index_;
	marshaller >> args.leader_commit_;
	marshaller >> args.entries_;
	return marshaller;
}

inline Marshaller& operator<<(Marshaller &marshaller, const AppendEntryReply &reply)
{
	marshaller << reply.term_;
	marshaller << reply.success_;
	marshaller << reply.curr_idx_;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, AppendEntryReply &reply)
{
	marshaller >> reply.term_;
	marshaller >> reply.success_;
	marshaller >> reply.curr_idx_;
	return marshaller;
}

} // namespace Mushroom

#endif /* _RAFT_ARG_HPP_ */