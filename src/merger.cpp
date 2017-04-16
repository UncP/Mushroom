/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-14 11:37:39
**/

#ifndef NOLSM

#include <queue>
#include <vector>

#include "merger.hpp"
#include "sstable.hpp"
#include "sstable_manager.hpp"

namespace Mushroom {

struct Tuple
{
	Tuple():iter(0), idx(0) { }
	Tuple(SSTable::Iterator *iter, uint32_t idx):iter(iter), idx(idx) { }

	SSTable::Iterator *iter;
	uint32_t  idx;
};

static auto greater = [](const Tuple &lhs, const Tuple &rhs) {
	return lhs.iter->key() > rhs.iter->key();
};

void Merge(SSTable **sstables, uint32_t size, SSTableManager *sstable_manager,
	BlockManager *block_manager)
{
	SSTable::Iterator *iters[size];
	std::vector<Tuple> tuples(size);
	for (uint32_t i = 0; i != size; ++i) {
		iters[i] = new SSTable::Iterator(sstables[i]);
		tuples[i] = Tuple(iters[i], i);
	}

	std::priority_queue<Tuple, std::vector<Tuple>, decltype(greater)> queue(greater, tuples);

	uint32_t level = 0;
	uint32_t key_len = tuples[0].iter->key().size_;
	SSTable *sstable = sstable_manager->NewSSTable(level, key_len);

	for (; !queue.empty();) {
		Tuple tuple = queue.top();
		if (!sstable->Append(tuple.iter->key(), block_manager)) {
			sstable = sstable_manager->NewSSTable(level, key_len);
			assert(sstable->Append(tuple.iter->key(), block_manager));
		}
		queue.pop();
		if (iters[tuple.idx]->Next())
			queue.push(tuple);
	}

	for (size_t i = 0; i != size; ++i)
		delete iters[i];
}

} // namespace Mushroom

#endif /* NOLSM */