/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:40
**/

#include "block.hpp"

namespace Mushroom {

Block::Block(block_t block_no)
:block_no_(block_no), mem_(new char[BlockSize]), num_((uint32_t *)mem_), off_(4)
{
	*num_ = 0;
}

Block::~Block() { delete [] mem_; }

block_t Block::BlockNo() const { return block_no_; }

const char* Block::Memory() const { return mem_ + 4; }

uint32_t Block::TotalKey() const { return *num_; }

bool Block::Append(const char *data, uint32_t len) {
	if (off_ + len < BlockSize) {
		memcpy(mem_ + off_, data, len);
		off_ += len;
		++*num_;
		return true;
	}
	return false;
}

void Block::Reset(block_t block_no)
{
	block_no_ = block_no;
	*num_ = 0;
	off_  = 4;
}

std::string Block::ToString() const
{
	return std::to_string(*num_);
}

} // namespace Mushroom
