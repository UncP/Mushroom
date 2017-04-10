/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 16:56:06
**/

#include "sktable.hpp"
#include "b_link_tree.hpp"
#include "block.hpp"

namespace Mushroom {

SKTable::SKTable(const BLinkTree *b_link_tree)
:b_link_tree_(b_link_tree), total_(0), head_(0), smallest_(0), largest_(0) { }

void SKTable::Compact()
{
	BLinkTree::Iterator iter(b_link_tree_);
	assert(iter.Begin() && !head_);
	head_ = new Block();
	Block *curr = head_;
	uint32_t len = btree_->KeyLength();
	for (;;) {
		if (!curr->Append((char *)iter.key_, len)) {
			curr->next_ = new Block();
			curr = curr->next_;
		} else if (!iter.Next()) {
			break;
		}
	}
}

} // namespace Mushroom
