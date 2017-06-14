/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-03 10:51:13
**/

#ifndef _MUSHROOM_LOG_HPP_
#define _MUSHROOM_LOG_HPP_

#include <string>

#include "../include/utility.hpp"
#include "../rpc/marshaller.hpp"
#include "../mushroom/slice.hpp"

namespace Mushroom {

struct MushroomLog : private NoCopy
{
	static const uint32_t LogSize = KeySlice::KeySize + sizeof(uint32_t);

	MushroomLog& operator=(const MushroomLog &that) {
		term_ = that.term_;
		memcpy(key_, that.key_, KeySlice::KeySize);
		return *this;
	}

	std::string ToString() const {
		return std::to_string(term_) + ' ' + key_->ToString();
	}

	uint32_t term_;
	KeySlice key_[0];
};

inline MushroomLog* NewMushroomLog() {
	return (MushroomLog *)new char[MushroomLog::LogSize];
}

inline void DeleteMushroomLog(MushroomLog *log) {
	delete [] (char *)log;
}

inline Marshaller& operator<<(Marshaller &marshaller, const MushroomLog &log)
{
	marshaller << log.term_;
	marshaller.Read(log.key_, KeySlice::KeySize);
	return marshaller;
}

} // namespace Mushroom

#endif /* _MUSHROOM_LOG_HPP_ */