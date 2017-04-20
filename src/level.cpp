/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:54:57
**/

#include <cassert>

#include "level.hpp"
#include "slice.hpp"
#include "sstable.hpp"
#include "sstable_manager.hpp"

namespace Mushroom {

Level::Level(uint32_t level):level_(level) { }

Level::~Level()
{
	printf("\033[34mlevel: %u  size: %lu\033[0m\n", level_, sstables_.size());
	for (uint32_t i = 0; i != sstables_.size(); ++i)
		printf("%s\n", sstables_[i]->ToString().c_str());
}

uint32_t Level::SSTableNumber() const
{
	return sstables_.size();
}

std::vector<SSTable *> Level::SSTables() const
{
	return sstables_;
}

void Level::AppendSSTable(SSTable *sstable)
{
	sstables_.push_back(sstable);
}

void Level::GetKeyRange(Key *smallest, Key *largest) const
{
	assert(sstables_.size());
	Key tmp1(smallest->size_), tmp2(largest->size_);
	sstables_[0]->GetKeyRange(smallest, largest);
	for (uint32_t i = 1; i != sstables_.size(); ++i) {
		sstables_[i]->GetKeyRange(&tmp1, &tmp2);
		if (tmp1 < *smallest) *smallest = tmp1;
		if (*largest < tmp2)  *largest  = tmp2;
	}
}

void Level::FindOverlapSSTable(std::vector<SSTable *> *tables, uint32_t *index, uint32_t *total,
	const Key &smallest, const Key &largest) const
{
	*index = sstables_.size();
	*total = 0;
	for (uint32_t i = 0; i < sstables_.size(); ++i) {
		if (sstables_[i]->Overlap(smallest, largest)) {
			tables->push_back(sstables_[i]);
			if (!*total) *index = i;
			++*total;
		} else if (*total) {
			break;
		}
	}
}

SSTable* Level::NextSSTable(Key &offset, uint32_t *index) const
{
	for (uint32_t i = 0; i < sstables_.size(); ++i) {
		if (sstables_[i]->LargerThan(offset)) {
			*index = i;
			return sstables_[i];
		}
	}
	assert(0);
}

void Level::DeleteSSTable(uint32_t index, uint32_t total, SSTableManager *sstable_manager)
{
	assert(index + total <= sstables_.size());
	for (uint32_t i = index; i < index + total; ++i)
		sstable_manager->FreeSSTable(sstables_[i]->TableNo());
	auto begin = sstables_.begin() + index;
	sstables_.erase(begin, begin + total);
}

bool Level::UpdateSSTable(uint32_t index, uint32_t total, const std::vector<SSTable *> &result,
	SSTableManager *sstable_manager)
{
	DeleteSSTable(index, total, sstable_manager);
	sstables_.insert(sstables_.begin() + index, result.begin(), result.end());
	return ReachThreshold();
}

bool Level::ReachThreshold()
{
	assert(level_);
	return sstables_.size() >= (uint32_t(1) << (level_ + 1));
}

} // namespace Mushroom
