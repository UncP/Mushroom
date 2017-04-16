/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#ifndef NOLSM

#include <cstring>

#include "lsm_tree.hpp"
#include "b_link_tree.hpp"
#include "sstable.hpp"
#include "sstable_manager.hpp"

namespace Mushroom {

LSMTree::LSMTree(uint32_t component, uint32_t key_len)
:component_(component - 1), key_len_(key_len), mem_tree_(new BLinkTree(key_len_)),
 imm_tree_(0), disk_trees_(new BLinkTree*[component_]),
 sstable_manager_(new SSTableManager()) { }

LSMTree::~LSMTree()
{
	delete mem_tree_;
	if (imm_tree_)
		delete imm_tree_;

	delete [] disk_trees_;

	delete sstable_manager_;
}

bool LSMTree::Free()
{
	mem_tree_->Free();
	if (imm_tree_)
		imm_tree_->Free();

	return true;
}

bool LSMTree::Put(KeySlice *key)
{
	if (mem_tree_->ReachThreshold()) SwitchMemoryTree();
	return mem_tree_->Put(key);
}

bool LSMTree::Get(KeySlice *key) const
{
	if (!mem_tree_->Get(key))
		if (imm_tree_ && !imm_tree_->Get(key))
			return false;
	return true;
}

void LSMTree::SwitchMemoryTree()
{
	#ifndef NOLATCH
	spin_.Lock();
	#endif
	if (mem_tree_->ReachThreshold()) {
		BLinkTree *new_tree = imm_tree_;
		#ifndef NOLATCH
		mutex_.Lock();
		#endif
		imm_tree_ = mem_tree_;
		if (!new_tree) {
			mem_tree_ = new BLinkTree(key_len_);
		} else {
			new_tree->Reset();
			mem_tree_ = new_tree;
		}
		#ifndef NOLATCH
		spin_.Unlock();
		#endif
		imm_tree_->Clear();
		if (sstable_manager_->ReachThreshold()) {
			sstable_manager_->MergeDirectSSTable();
			Merge();
		}
		sstable_manager_->AddDirectSSTable(imm_tree_);
		#ifndef NOLATCH
		mutex_.Unlock();
		#endif
	} else {
		#ifndef NOLATCH
		spin_.Unlock();
		#endif
	}
}

void LSMTree::Merge()
{

}

} // namespace Mushroom

#endif /* NOLSM */
