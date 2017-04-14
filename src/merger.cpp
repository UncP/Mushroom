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

static auto compare = [](const Tuple &lhs, const Tuple &rhs) {
	return lhs.iter->key() > rhs.iter->key();
};

SSTable* Merge(const table_t *tables, uint32_t size, SSTableManager *sstable_manager,
	BlockManager *block_manager)
{
	SSTable::Iterator *iters[size];
	std::vector<Tuple> tuples(size);
	for (uint32_t i = 0; i != size; ++i) {
		iters[i] = new SSTable::Iterator(sstable_manager->GetSSTable(tables[i]));
		tuples[i] = Tuple(iters[i], i);
	}

	std::priority_queue<Tuple, std::vector<Tuple>, decltype(compare)> queue(compare, tuples);

	SSTable *sstable = sstable_manager->NewSSTable();

	// for (; !queue.empty();) {
	// 	Tuple tuple = queue.top();
	// 	sstable->
	// 	queue.pop();
	// 	if (iters[tuple.idx_]->Next())
	// 		queue.push(tuple);
	// }

	for (size_t i = 0; i != size_; ++i)
		delete iters[i];
}

} // namespace Mushroom

#endif /* NOLSM */