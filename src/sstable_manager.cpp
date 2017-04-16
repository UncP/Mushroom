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
#include "block_manager.hpp"
#include "merger.hpp"

namespace Mushroom {

SSTableManager::SSTableManager():cur_(0), block_manager_(new BlockManager()) { }

SSTableManager::~SSTableManager()
{
	printf("sstable size: %lu\n", sstables_.size());
	for (size_t i = 0; i != sstables_.size(); ++i)
		delete sstables_[i];
	delete block_manager_;
}

void SSTableManager::AddDirectSSTable(const BLinkTree *b_link_tree)
{
	SSTable *sstable = dir_[cur_++];
	if (!sstable) {
		sstable = new SSTable();
		dir_[cur_-1] = sstable;
	}
	sstable->Generate(b_link_tree, block_manager_);
}

void SSTableManager::MergeDirectSSTable()
{
	printf("merge\n");
	Merge(dir_, MaxDirectSSTable, this, block_manager_);
	for (uint32_t i = 0; i != cur_; ++i)
		dir_[i]->Reset();
	cur_ = 0;
}

SSTable* SSTableManager::NewSSTable(uint32_t level, uint32_t key_len)
{
	SSTable *sstable;
	if (!free_.empty()) {
		table_t table_no = free_.top();
		free_.pop();
		sstable = GetSSTable(table_no);
		sstable->Reset(table_no, level, key_len);
	} else {
		sstable = new SSTable(sstables_.size(), level, key_len);
		sstables_.push_back(sstable);
	}
	return sstable;
}

SSTable* SSTableManager::GetSSTable(table_t table_no) const
{
	assert(table_no < sstables_.size());
	return sstables_[table_no];
}

} // namespace Mushroom

#endif /* NOLSM */
