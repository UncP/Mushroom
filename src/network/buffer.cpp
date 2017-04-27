/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-27 09:44:47
**/

#include <cstring>
#include <cassert>

#include "buffer.hpp"

namespace Mushroom {

Buffer::Buffer():data_(new char[BufferSize]), size_(0), beg_(0), end_(0) { }

Buffer::~Buffer()
{
	delete data_;
}

uint32_t Buffer::size() const
{
	return size_;
}

char* Buffer::begin() const
{
	return data_ + beg_;
}

char* Buffer::end() const
{
	return data_ + end_;
}

void Buffer::Append(const char *data, uint32_t len)
{
	assert((end_ + len) <= BufferSize);
	memcpy(data_ + beg_, data, len);
}

void Buffer::Consume(uint32_t len)
{
	beg_ += len;
}

} // namespace Mushroom
