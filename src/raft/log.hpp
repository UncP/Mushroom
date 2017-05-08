/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-07 22:19:41
**/

#ifndef _RAFT_LOG_HPP_
#define _RAFT_LOG_HPP_

#include "../rpc/marshaller.hpp"

namespace Mushroom {

struct Log
{
		Log() { }

		Log(uint32_t term, uint32_t number):term_(term), number_(number) { }

		uint32_t term_;
		uint32_t number_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const Log &log)
{
	marshaller << log.term_;
	marshaller << log.number_;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, Log &log)
{
	marshaller >> log.term_;
	marshaller >> log.number_;
	return marshaller;
}

} // namespace Mushroom

#endif /* _RAFT_LOG_HPP_ */