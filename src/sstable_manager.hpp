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
#include <stack>

#include "utility.hpp"

namespace Mushroom {

class SSTableManager
{
	public:
		static const uint32_t MaxDirectSSTable = 4;

		SSTableManager();

		~SSTableManager();

		bool ReachThreshold() { return cur_ == MaxDirectSSTable; }

		void MergeDirectSSTable();

		void AddDirectSSTable(const BLinkTree *b_link_tree);

		SSTable* NewSSTable(uint32_t level, uint32_t key_len);

		SSTable* GetSSTable(table_t table_no) const;

	private:
		uint32_t               cur_;
		SSTable               *dir_[MaxDirectSSTable];
		BlockManager          *block_manager_;
		std::stack<SSTable *>  free_;
		std::vector<SSTable *> sstables_;
};

} // namespace Mushroom

#endif /* _SSTABLE_MANAGER_HPP_ */

#endif /* NOLSM */