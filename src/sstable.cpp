/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-08 16:56:06
**/

#include "sstable.hpp"
#include "b_link_tree.hpp"

namespace Mushroom {

SSTable::SSTable(const BLinkTree *b_link_tree):b_link_tree_(b_link_tree) { }

void SSTable::Compact()
{
	BLinkTree::Iterator iter(b_link_tree_);
	assert(iter.Begin());
	for (; iter.Next(); ) {

	}
}

} // namespace Mushroom
