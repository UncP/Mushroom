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
#include "merger.hpp"

namespace Mushroom {

SSTableManager::SSTableManager():cur_(0) { }

SSTableManager::~SSTableManager()
{
	for (size_t i = 0; i != sstables_.size(); ++i)
		delete sstables_[i];
}

void SSTableManager::AddDirectSSTable(const BLinkTree *b_link_tree, BlockManager *block_manager)
{
	SSTable *sstable;
	if (!free_.empty()) {
		sstable = free_.top();
		free_.pop();
	} else {
		sstable = new
	}
	dir_[cur_] = new SSTable(b_link_tree, block_manager);
	++cur_;
}

void SSTableManager::MergeDirectSSTable(BlockManager *block_manager)
{
	Merge(dir_, MaxDirectSSTable, this, block_manager);
	for (uint32_t i = 0; i != cur_; ++i)
		free_.push(dir_[i]);
	cur_ = 0;
}

SSTable* SSTableManager::NewSSTable(uint32_t level, uint32_t key_len)
{
	SSTable *sstable;
	if (!free_.empty()) {
		sstable = free_.top();
		free_.pop();
		sstable->Reset(level, key_len, sstables_.size());
	} else {
		sstable = new SSTable(level, key_len, sstables_.size());
	}
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