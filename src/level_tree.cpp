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

uint32_t LevelTree::MaxLevel0SSTable = 4;

LevelTree::LevelTree(uint32_t key_len)
:key_len_(key_len), sstable_manager_(new SSTableManager()), merger_(new Merger())
{
	AppendNewLevel();
}

LevelTree::~LevelTree()
{
	for (int32_t i = int32_t(levels_.size()) - 1; i >= 0; --i)
		delete levels_[i];

	delete merger_;
	delete sstable_manager_;
}

void LevelTree::AppendLevel0SSTable(const BLinkTree *b_link_tree)
{
	SSTable *sstable = sstable_manager_->NewSSTable(0);
	sstable->Generate(b_link_tree, sstable_manager_->block_manager_);
	levels_[0]->AppendSSTable(sstable);
	if (levels_[0]->SSTableNumber() >= MaxLevel0SSTable)
		MergeLevel(0);
}

void LevelTree::MergeLevel(uint32_t level)
{
	// printf("merge level: %u\n", level);
	Key smallest(key_len_), largest(key_len_);
	std::vector<SSTable *> tables;
	uint32_t index1, total1, index2 = 0, total2 = 0;
	if (!level) {
		tables = levels_[0]->SSTables();
		index1 = 0;
		total1 = tables.size();
		GetKeyRangeInLevel(level, &smallest, &largest);
	} else {
		const Key &offset = merger_->GetOffsetInLevel(level);
		tables.push_back(NextSSTableInLevel(level + 1, offset, &index1));
		total1 = 1;
		tables[0]->GetKeyRange(&smallest, &largest);
	}
	if (++level >= levels_.size()) AppendNewLevel();
	// printf("find overlap\n");
	FindOverlapInLevel(level, &tables, &index2, &total2, smallest, largest);
	std::vector<SSTable *> result;
	// printf("do merge\n");
	merger_->Merge(tables, sstable_manager_, sstable_manager_->block_manager_, level, key_len_,
		&result);
	// printf("update sstable\n");
	UpdateSSTableInLevel(level, index2, total2, result);
	// printf("delete sstable\n");
	DeleteSSTableInLevel(level - 1, index1, total1);
}

void LevelTree::GetKeyRangeInLevel(uint32_t level, Key *smallest, Key *largest) const
{
	assert(level < levels_.size());
	levels_[level]->GetKeyRange(smallest, largest);
}

void LevelTree::FindOverlapInLevel(uint32_t level, std::vector<SSTable *> *tables,
	uint32_t *index, uint32_t *total, const Key &smallest, const Key &largest) const
{
	assert(level < levels_.size());
	levels_[level]->FindOverlapSSTable(tables, index, total, smallest, largest);
}

SSTable* LevelTree::NextSSTableInLevel(uint32_t level, const Key &offset, uint32_t *index) const
{
	assert(level < levels_.size());
	return levels_[level]->NextSSTable(offset, index);
}

void LevelTree::UpdateSSTableInLevel(uint32_t level, uint32_t index, uint32_t total,
	const std::vector<SSTable *> &result)
{
	assert(level < levels_.size());
	levels_[level]->UpdateSSTable(index, total, result, sstable_manager_);
}

void LevelTree::DeleteSSTableInLevel(uint32_t level, uint32_t index, uint32_t total)
{
	assert(level < levels_.size());
	levels_[level]->DeleteSSTable(index, total, sstable_manager_);
}

void LevelTree::AppendNewLevel()
{
	uint32_t level = levels_.size();
	levels_.push_back(new Level(level));
	merger_->AppendMergePointer(key_len_);
}

} // namespace Mushroom

#endif /* NOLSM */
