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

namespace Mushroom {

SSTableManager::SSTableManager():block_manager_(new BlockManager()) { }

SSTableManager::~SSTableManager()
{
	printf("SSTableManager Destructor\ntotal: %lu  free: %lu\n", sstables_.size(), free_.size());
	for (uint32_t i = 0; i != sstables_.size(); ++i)
		delete sstables_[i];

	delete block_manager_;
}

SSTable* SSTableManager::NewSSTable(uint32_t level)
{
	SSTable *sstable;
	if (!free_.empty()) {
		table_t table_no = free_.top();
		free_.pop();
		sstable = GetSSTable(table_no);
		sstable->Reset(table_no, level);
	} else {
		sstable = new SSTable(sstables_.size(), level);
		sstables_.push_back(sstable);
	}
	return sstable;
}

SSTable* SSTableManager::GetSSTable(table_t table_no) const
{
	assert(table_no < sstables_.size());
	return sstables_[table_no];
}

void SSTableManager::FreeSSTable(table_t table_no)
{
	assert(table_no < sstables_.size());
	GetSSTable(table_no)->Free(block_manager_);
	free_.push(table_no);
}

} // namespace Mushroom

#endif /* NOLSM */
