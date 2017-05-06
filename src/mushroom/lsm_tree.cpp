/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#include <cstring>

#include "lsm_tree.hpp"
#include "b_link_tree.hpp"
#include "level_tree.hpp"

namespace Mushroom {

LSMTree::LSMTree(uint32_t key_len):key_len_(key_len), mem_tree_(new BLinkTree(key_len_)),
 imm_tree_(0), lvl_tree_(new LevelTree(key_len_)) { }

LSMTree::~LSMTree()
{
	delete mem_tree_;
	if (imm_tree_)
		delete imm_tree_;

	delete lvl_tree_;
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

bool LSMTree::Get(KeySlice *key)
{
	if (!mem_tree_->Get(key))
		if (imm_tree_ && !imm_tree_->Get(key))
			return false;
	return true;
}

void LSMTree::SwitchMemoryTree()
{
	spin_.Lock();
	if (mem_tree_->ReachThreshold()) {
		BLinkTree *new_tree = imm_tree_;
		mutex_.Lock();
		imm_tree_ = mem_tree_;
		if (!new_tree) {
			__sync_bool_compare_and_swap(&mem_tree_, mem_tree_, new BLinkTree(key_len_));
		} else {
			new_tree->Reset();
			__sync_bool_compare_and_swap(&mem_tree_, mem_tree_, new_tree);
		}
		spin_.Unlock();
		imm_tree_->Clear();
		lvl_tree_->AppendLevel0SSTable(imm_tree_);
		mutex_.Unlock();
	} else {
		spin_.Unlock();
	}
}

} // namespace Mushroom
