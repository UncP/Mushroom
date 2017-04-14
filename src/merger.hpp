/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-14 11:37:32
**/

#ifndef NOLSM

#ifndef _MERGER_HPP_
#define _MERGER_HPP_

#include "utility.hpp"

namespace Mushroom {

SSTable* DoMerge(const table_t *tables, uint32_t size, SSTableManager *sstable_manager,
	BlockManager *block_manager);

} // namespace Mushroom

#endif /* _MERGER_HPP_ */

#endif /* NOLSM */