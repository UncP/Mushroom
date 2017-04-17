/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:54:57
**/

#ifndef NOLSM

#include "level.hpp"

namespace Mushroom {

Level::Level(uint32_t level):level_(level) { }

uint32_t Level::SSTableNumber() const
{
	return sstables_.size();
}

} // namespace Mushroom

#endif /* NOLSM */
