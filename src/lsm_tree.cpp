/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-01 13:22:35
**/

#include "lsm_tree.hpp"

namespace Mushroom {

LSMTree::LSMTree(BLinkTree *small, BLinkTree *big):small_(small), big_(big) { }

LSMTree::~LSMTree()
{
	delete small_;
	delete big_;
}

LSMTree::Free()
{
	small_->Free();
	big_->Free();
}

} // namespace Mushroom
