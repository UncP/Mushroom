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
#include "block_manager.hpp"
#include "sstable_manager.hpp"

namespace Mushroom {

LSMTree::LSMTree(uint32_t component, uint32_t key_len)
:component_(component - 1), key_len_(key_len), curr_(0), mem_tree_(new BLinkTree(key_len_)),
 imm_tree_(0), disk_trees_(new BLinkTree*[component_]), block_manager_(new BlockManager()),
 sstable_manager_(new SSTableManager()) { }

LSMTree::~LSMTree()
{
	delete mem_tree_;
	if (imm_tree_)
		delete imm_tree_;

	for (uint32_t i = 0; i != curr_; ++i)
		delete disk_trees_[i];
	delete [] disk_trees_;

	delete block_manager_;
	delete sstable_manager_;
}

bool LSMTree::Free()
{
	mem_tree_->Free();
	if (imm_tree_)
		imm_tree_->Free();

	for (uint32_t i = 0; i != curr_; ++i)
		disk_trees_[i]->Free();
	return true;
}

bool LSMTree::Put(KeySlice *key)
{
	if (mem_tree_->ReachThreshold()) {
		#ifndef NOLATCH
		mutex_.Lock();
		#endif
		if (mem_tree_->ReachThreshold()) {
			BLinkTree *new_tree = imm_tree_;
			imm_tree_ = mem_tree_;
			if (!new_tree)
				mem_tree_ = new BLinkTree(key_len_);
			else
				mem_tree_ = new_tree;
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
			imm_tree_->Clear();
			SSTable *sstable = sstable_manager_->NewSSTable(imm_tree_, block_manager_);
			imm_tree_->Reset();
			// Merge(table);
		} else {
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
		}
	}
	return mem_tree_->Put(key);
}

bool LSMTree::Get(KeySlice *key) const
{
	if (!mem_tree_->Get(key))
		if (imm_tree_ && !imm_tree_->Get(key))
			return false;
	return true;
}

// void Merge(const SSTable *table)
// {
	// uint32_t idx = curr_;
	// BLinkTree *cur = disk_trees_[idx];
	// TempSlice(slice, (cur->KeyLength() << 1));
	// table->GetKeyRange(slice);
// }

} // namespace Mushroom

#endif
