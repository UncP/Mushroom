/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-17 14:54:57
**/

#ifndef NOLSM

#include "level.hpp"
#include "slice.hpp"

namespace Mushroom {

Level::Level(uint32_t level):level_(level) { }

uint32_t Level::SSTableNumber() const
{
	return sstables_.size();
}

void Level::FindOverlapSSTable(std::vector<SSTable *> *tables, const Key &smallest,
	const Key &largest, uint32_t *index, uint32_t *total) const
{
	*index = sstables_.size();
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

SSTable* Level::NextSSTable(const Key &offset, uint32_t *index) const
{
	for (uint32_t i = 0; i < sstables_.size(); ++i) {
		if (sstables_[i]->LargerThan(offset)) {
			*index = i;
			return sstables_[i];
		}
	}
}

} // namespace Mushroom

#endif /* NOLSM */
