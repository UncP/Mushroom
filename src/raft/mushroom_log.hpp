/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-03 10:51:13
**/

#ifndef _MUSHROOM_LOG_HPP_
#define _MUSHROOM_LOG_HPP_

#include "../include/utility.hpp"
#include "../rpc/marshaller.hpp"
#include "../mushroom/slice.hpp"

namespace Mushroom {

struct MushroomLog
{
	MushroomLog():key_(NewKeySlice()) { }

	MushroomLog(const MushroomLog &that) {
		term_ = that.term_;
		key_  = NewKeySlice();
		memcpy(key_, that.key_, KeySlice::KeySize);
	}

	MushroomLog& operator=(const MushroomLog &that) {
		term_ = that.term_;
		key_  = NewKeySlice();
		memcpy(key_, that.key_, KeySlice::KeySize);
		return *this;
	}

	~MushroomLog() { DeleteKeySlice(key_); }

	uint32_t  term_;
	KeySlice *key_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const MushroomLog &log)
{
	marshaller << log.term_;
	marshaller.Read(log.key_, KeySlice::KeySize);
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, MushroomLog &log)
{
	marshaller >> log.term_;
	log.key_ = NewKeySlice();
	marshaller.Write(log.key_, KeySlice::KeySize);
	return marshaller;
}

} // namespace Mushroom

#endif /* _MUSHROOM_LOG_HPP_ */