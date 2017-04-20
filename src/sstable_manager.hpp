/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-12 22:22:46
**/

#ifndef _SSTABLE_MANAGER_HPP_
#define _SSTABLE_MANAGER_HPP_

#include <vector>
#include <stack>

#include "utility.hpp"

namespace Mushroom {

class SSTableManager
{
	public:
		friend class LevelTree;

		SSTableManager();

		~SSTableManager();

		SSTable* NewSSTable(uint32_t level);

		SSTable* GetSSTable(table_t table_no) const;

		void FreeSSTable(table_t table_no);

	private:
		std::vector<SSTable *> sstables_;
		std::stack<table_t>    free_;
		BlockManager          *block_manager_;
};

} // namespace Mushroom

#endif /* _SSTABLE_MANAGER_HPP_ */