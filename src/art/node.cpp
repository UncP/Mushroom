/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-30 10:10:10
**/

#include "node.hpp"

namespace Mushroom {

Leaf* Node::Minimum(const Node *node)
{
	if (!node) return 0;
	if (IS_LEAF(node)) return LEAF_RAW(node);

	int idx;
	union {
		const Node48  *p48;
		const Node256 *p256;
	}p;
	switch (node->Type()) {
		case NODE4:
			return Minimum(((const Node4 *)node)->ChildAt(0));
		case NODE16:
			return Minimum(((const Node16 *)node)->ChildAt(0));
		case NODE48:
			idx = 0;
			p.p48 = (const Node48 *)node;
			while (!p.p48->KeyAt(idx)) ++idx;
			idx = p.p48->KeyAt(idx);
			return Minimum(p.p48->ChildAt(idx));
		case NODE256:
			idx = 0;
			p.p256 = (const Node256 *)node;
			while (!p.p256->ChildAt(idx)) ++idx;
			return Minimum(p.p256->ChildAt(idx));
		default:
			assert(0);
	}
}

} // namespace Mushroom
