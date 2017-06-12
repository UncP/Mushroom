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
	MushroomLog():key_(0) { }

	MushroomLog(uint8_t len):len_(len), key_(NewKeySlice(len_)) { }

	MushroomLog(const MushroomLog &that) {
		term_ = that.term_;
		len_  = that.len_;
		key_  = NewKeySlice(len_);
		CopyKey(key_, that.key_, 0, len_);
	}

	MushroomLog& operator=(const MushroomLog &that) {
		if (len_ != that.len_) {
			delete [] (char *)key_;
			len_  = that.len_;
			key_  = NewKeySlice(len_);
		}
		term_ = that.term_;
		CopyKey(key_, that.key_, 0, len_);
		return *this;
	}

	~MushroomLog() {
		delete [] (char *)key_;
	}

	uint32_t  term_;
	uint8_t   len_;
	KeySlice *key_;
};

inline Marshaller& operator<<(Marshaller &marshaller, const MushroomLog &log)
{
	marshaller << log.term_;
	marshaller << log.len_;
	marshaller.Read(log.key_, sizeof(page_t) + log.len_);
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, MushroomLog &log)
{
	marshaller >> log.term_;
	marshaller >> log.len_;
	log.key_ = NewKeySlice(log.len_);
	marshaller.Write(log.key_, sizeof(page_t) + log.len_);
	return marshaller;
}

} // namespace Mushroom

#endif /* _MUSHROOM_LOG_HPP_ */