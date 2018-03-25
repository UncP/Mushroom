/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-30 10:10:10
**/

#include <cassert>

#include "node.hpp"

namespace Mushroom {

uint32_t Node::MAX_PREFIX_LEN = 8;

void Free(Node *node)
{
	if (!node) return ;

	if (IS_LEAF(node)) {
		DeleteLeaf(LEAF_RAW(node));
		return ;
	}

	int i;
	union {
		Node4   *p4;
		Node16  *p16;
		Node48  *p48;
		Node256 *p256;
	}p;
	switch (node->Type()) {
		case NODE4:
			p.p4 = (Node4 *)node;
			for (i = 0; i < node->Count(); ++i)
				Free(p.p4->ChildAt(i));
			break;
		case NODE16:
			p.p16 = (Node16 *)node;
			for (i = 0; i < node->Count(); ++i)
				Free(p.p16->ChildAt(i));
			break;
		case NODE48:
			p.p48 = (Node48 *)node;
			for (i = 0; i < node->Count(); ++i)
				Free(p.p48->ChildAt(i));
			break;
		case NODE256:
			p.p256 = (Node256 *)node;
			for (i = 0; i < 256; ++i)
				if (p.p256->ChildAt(i))
					Free(p.p256->ChildAt(i));
			break;
		default:
			assert(0);
	}
	delete node;
}

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
			idx = p.p48->KeyAt(idx) - 1;
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
