/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 21:03:40
**/

#include "block.hpp"

namespace Mushroom {

Block::Block():mem_(new char[BlockSize]), off_(0), prev_(0), next_(0) { }

Block::~Block() { delete [] mem_; }

} // namespace Mushroom
