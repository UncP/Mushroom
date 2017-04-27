/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-27 09:44:47
**/

#include "buffer.hpp"

namespace Mushroom {

Buffer::Buffer():data_(new char[BufferSize]), size_(0), beg_(0), end_(0) { }

Buffer::~Buffer()
{
	delete data_;
}

} // namespace Mushroom
