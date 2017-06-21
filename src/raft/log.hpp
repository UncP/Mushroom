/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-07 22:19:41
**/

#ifndef _RAFT_LOG_HPP_
#define _RAFT_LOG_HPP_

#include "../rpc/marshaller.hpp"

namespace Mushroom {

struct Log
{
		Log() { }
		Log(uint32_t number):number_(number) { }

		bool operator!=(const Log& that) { return term_ != that.term_ || number_ != that.number_; }

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