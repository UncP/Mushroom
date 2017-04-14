/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-12 22:22:46
**/

#ifndef NOLSM

#ifndef _SSTABLE_MANAGER_HPP_
#define _SSTABLE_MANAGER_HPP_

#include <vector>

#include "utility.hpp"

namespace Mushroom {

class SSTableManager
{
	public:
		SSTableManager();

		~SSTableManager();

		SSTable* NewSSTable(const BLinkTree *b_link_tree, BlockManager *block_manager);

		SSTable* NewSSTable();

		SSTable* GetSSTable(table_t table_no) const;

	private:
		std::vector<SSTable *> sstables_;
};

} // namespace Mushroom

#endif /* _SSTABLE_MANAGER_HPP_ */

#endif /* NOLSM */