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


}

void LevelTree::AppendNewLevel()
{
	uint32_t level = levels_.size();
	levels_.push_back(level);
	merger->AppendMergePointer(key_len_);
}

} // namespace Mushroom

#endif /* NOLSM */
