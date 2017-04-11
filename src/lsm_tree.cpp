/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#ifndef NOLSM

#include "lsm_tree.hpp"
#include "b_link_tree.hpp"
#include "sstable.hpp"

namespace Mushroom {

LSMTree::LSMTree(int component, int key_len)
:component_(component), key_len_(key_len), curr_(1), trees_(new BLinkTree*[component_])\
{
	trees_[0] = new BLinkTree(key_len_);
}

LSMTree::~LSMTree()
{
	for (uint32_t i = 0; i != curr_; ++i)
		delete trees_[i];
	delete [] trees_;
}

bool LSMTree::Free()
{
	for (uint32_t i = 0; i != curr_; ++i)
		trees_[i]->Free();
	return true;
}

bool LSMTree::Put(KeySlice *key)
{
	if (trees_[0]->ReachThreshold()) {
		#ifndef NOLATCH
		mutex_.Lock();
		#endif
		if (trees_[0]->ReachThreshold()) {
			BLinkTree *old_tree = trees_[0];
			BLinkTree *new_tree = new BLinkTree(key_len_);
			trees_[0] = new_tree;
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
			old_tree->Clear();
			SSTable *table = new SSTable(old_tree);
			old_tree->Free();
			delete old_tree;
			Merge(table);
		} else {
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
		}
	}
	return trees_[0]->Put(key);
}

bool LSMTree::Get(KeySlice *key) const
{
	// return trees_[0]->Get(key);
	return false;
}

void Merge(const SSTable *table)
{
	uint32_t idx = curr_;
	BLinkTree *cur = trees_[idx];
	TempSlice(slice, (cur->KeyLength() << 1));
	table->GetKeyRange(slice);
}

} // namespace Mushroom

#endif
