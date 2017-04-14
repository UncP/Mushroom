/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-12 22:25:05
**/

#ifndef NOLSM

#include <cassert>

#include "sstable_manager.hpp"
#include "sstable.hpp"

namespace Mushroom {

SSTableManager::SSTableManager() { }

SSTableManager::~SSTableManager()
{
	for (size_t i = 0; i != sstables_.size(); ++i)
		delete sstables_[i];
}

SSTable* SSTableManager::NewSSTable(const BLinkTree *b_link_tree, BlockManager *block_manager)
{
	SSTable *sstable = new SSTable(b_link_tree, block_manager, sstables_.size());
	sstables_.push_back(sstable);
	return sstable;
}

SSTable* SSTableManager::NewSSTable(uint32_t key_len)
{
	SSTable *sstable = new SSTable(key_len, sstables_.size());
	sstables_.push_back(sstable);
	return sstable;
}

SSTable* SSTableManager::GetSSTable(table_t table_no) const
{
	assert(table_no < sstables_.size());
	return sstables_[table_no];
}

} // namespace Mushroom

#endif /* NOLSM */