/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-14 15:09:28
**/

#include <cassert>

#include "mushroom_log_vector.hpp"

namespace Mushroom {

uint32_t LogVector::size() const { return size_; }

const MushroomLog& LogVector::operator[](uint32_t idx) const {
	return *(const MushroomLog *)(mem_ + MushroomLog::LogSize * idx);
}

MushroomLogVector::MushroomLogVector():size_(0), cap_(128), mem_(new char[cap_]) { }

MushroomLogVector::~MushroomLogVector() { delete [] mem_; }

uint32_t MushroomLogVector::size() const { return size_; }

uint32_t MushroomLogVector::cap() const { return cap_; }

MushroomLog& MushroomLogVector::operator[](uint32_t idx) const
{
	assert(idx < size_);
	return *(MushroomLog *)(mem_ + MushroomLog::LogSize * idx);
}

void MushroomLogVector::Append(const MushroomLog &log)
{
	if (++size_ * MushroomLog::LogSize > cap_) {
		char *buf = new char[cap_ << 1];
		memcpy(buf, mem_, cap_);
		cap_ <<= 1;
		delete [] mem_;
		mem_ = buf;
	}
	MushroomLog &l = (*this)[size_ - 1];
	l = log;
}

void MushroomLogVector::Append(const LogVector &that, uint32_t idx)
{
	for (uint32_t i = idx; i < that.size(); ++i)
		Append(that[i]);
}

void MushroomLogVector::DeleteFrom(uint32_t idx)
{
	assert(idx <= size_);
	size_ = idx;
}

void MushroomLogVector::FormLogVectorFrom(uint32_t idx, Buffer &buf) const
{
	assert(idx <= size_);
	uint32_t all = size_ - idx;
	uint32_t total = 4 + all * MushroomLog::LogSize;
	buf.Expand(total);
	buf.Read((const char *)&all, 4);
	if (all)
		buf.Read((const char *)&((*this)[idx]), total - 4);
}

} // namespace Mushroom
