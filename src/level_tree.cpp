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
	levels_[0].sstables_.push_back(sstable);
	if (levels_[0].SSTableNumber() >= MaxLevel0SSTable)
		MergeLevel(0);
}

void LevelTree::MergeLevel(uint32_t level)
{
	Key smallest(key_len_), largest(key_len_);
	std::vector<SSTable *> tables;
	uint32_t index, total = 0;
	if (!level) {
		tables = levels_[0].sstables;
		tables[0]->GetKeyRange(&smallest, &largest);
		FindOverlapInLevel(1, &tables, &index, &total, smallest, largest);
	} else {
		const Key &offset = merger_->GetOffsetInLevel(level);
		tables.push_back(NextSSTableInLevel(level + 1, offset, &index));
	}
	std::vector<SSTable *> result;
	merger_->Merge(tables, sstable_manager_, sstable_manager_->block_manager_, level + 1, &result);
	DeleteSSTableInLevel(level);
	AppendSSTableInLevel(level+1, result);
}

void LevelTree::FindOverlapInLevel(uint32_t level, std::vector<SSTable *> *tabels,
	uint32_t *index, uint32_t *total, const Key &smallest, const Key &largest)
{
	levels_[level].FindOverlapSSTable(tables, index, total, smallest, largest);
}

SSTable* LevelTree::NextSSTableInLevel(uint32_t level, const Key &offset, uint32_t *index)
{
	levels_[level].NextSSTable(offset, index);
}

void LevelTree::AppendNewLevel()
{
	uint32_t level = levels_.size();
	levels_.push_back(level);
	merger->AppendMergePointer(key_len_);
}

} // namespace Mushroom

#endif /* NOLSM */
