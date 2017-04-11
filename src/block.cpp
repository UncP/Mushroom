/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:40
**/

#ifndef NOLSM

#include "block.hpp"

namespace Mushroom {

Block::Iterator::Iterator(const Block *block, uint32_t key_len)
:key_(block->mem_+4), block_(block), key_len_(key_len) { }

Block::Block():mem_(new char[BlockSize]), num_((uint32_t *)mem_), off_(4) { }

Block::~Block() { delete [] mem_; }

} // namespace Mushroom

#endif
