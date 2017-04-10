/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#ifdef LSM

#include "lsm_tree.hpp"
#include "b_link_tree.hpp"

namespace Mushroom {

LSMTree::LSMTree(int key_len):key_len_(key_len)
{
	curr_ = new BLinkTree(key_len_);
}

LSMTree::~LSMTree()
{
	delete curr_;
}

bool LSMTree::Free()
{
	curr_->Free();
	return true;
}

bool LSMTree::Put(KeySlice *key)
{
	if (curr_->NeedCompact()) {
		#ifndef NOLATCH
		mutex_.Lock();
		#endif
		if (curr_->NeedCompact()) {
			BLinkTree *old_tree = curr_;
			BLinkTree *new_tree = new BLinkTree(key_len_);
			curr_ = new_tree;
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
			old_tree->Clear();
			// TODO
			old_tree->Free();
			delete old_tree;
		} else {
			#ifndef NOLATCH
			mutex_.Unlock();
			#endif
		}
	}
	return curr_->Put(key);
}

bool LSMTree::Get(KeySlice *key) const
{
	return curr_->Get(key);
}

} // namespace Mushroom

#endif
