/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:48:17
**/

#ifndef NOLSM

#include "level_tree.hpp"
#include "level.hpp"
#include "sstable.hpp"
#include "sstable_manager.hpp"
#include "merger.hpp"

namespace Mushroom {

LevelTree::LevelTree(uint32_t key_len)
:key_len_(key_len), sstable_manager_(new SSTableManager()), merger_(new Merger())
{
	AppendNewLevel();
}

void LevelTree::AppendLevel0SSTable(const BLinkTree *b_link_tree)
{
	SSTable *sstable = sstable_manager_->NewSSTable(0);
	sstable->Generate(b_link_tree, sstable_manager_->block_manager_);
	levels_[0].AppendSSTable(sstable);
	if (levels_[0].SSTableNumber() >= MaxLevel0SSTable)
		MergeLevel(0);
}

void LevelTree::MergeLevel(uint32_t level)
{
	Key smallest(key_len_), largest(key_len_);
	std::vector<SSTable *> tables;
	uint32_t index1, total1, index2, total2;
	if (!level) {
		tables = levels_[0].SSTables();
		index1 = 0;
		total1 = tables.size();
		GetKeyRangeInLevel(level, &smallest, &largest);
	} else {
		const Key &offset = merger_->GetOffsetInLevel(level);
		tables.push_back(NextSSTableInLevel(level + 1, offset, &index1));
		total1 = 1;
		tables[0]->GetKeyRange(&smallest, &largest);
	}
	FindOverlapInLevel(level + 1, &tables, &index2, &total2, smallest, largest);
	std::vector<SSTable *> result;
	merger_->Merge(tables, sstable_manager_, sstable_manager_->block_manager_, level + 1, key_len_,
		&result);
	UpdateSSTableInLevel(level+1, index2, total2, result);
	DeleteSSTableInLevel(level, index1, total1);
}

void LevelTree::GetKeyRangeInLevel(uint32_t level, Key *smallest, Key *largest) const
{
	levels_[level].GetKeyRange(smallest, largest);
}

void LevelTree::FindOverlapInLevel(uint32_t level, std::vector<SSTable *> *tables,
	uint32_t *index, uint32_t *total, const Key &smallest, const Key &largest) const
{
	levels_[level].FindOverlapSSTable(tables, index, total, smallest, largest);
}

SSTable* LevelTree::NextSSTableInLevel(uint32_t level, const Key &offset, uint32_t *index) const
{
	return levels_[level].NextSSTable(offset, index);
}

void LevelTree::UpdateSSTableInLevel(uint32_t level, uint32_t index, uint32_t total,
	const std::vector<SSTable *> &result)
{
	levels_[level].UpdateSSTable(index, total, result);
}

void LevelTree::DeleteSSTableInLevel(uint32_t level, uint32_t index, uint32_t total)
{
	levels_[level].DeleteSSTable(index, total);
}

void LevelTree::AppendNewLevel()
{
	uint32_t level = levels_.size();
	levels_.push_back(level);
	merger_->AppendMergePointer(key_len_);
}

} // namespace Mushroom

#endif /* NOLSM */
