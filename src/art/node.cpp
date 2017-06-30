/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-30 10:10:10
**/

namespace Mushroom {

Leaf* Node::Minimum(const Node *node)
{
	if (!node) return 0;
	if (IS_LEAF(node)) return LEAF_RAW(node);

	int idx;
	switch (node->type_) {
		case NODE4:
			return Minimum(((const Node4*)node)->ChildAt(0));
		case NODE16:
			return Minimum(((const art_node16*)n)->children[0]);
		case NODE48:
			idx = 0;
			while (!((const Node48 *)node)->KeyAt(idx)) ++idx;
			idx = ((const Node48 *)node)->KeyAt(idx);
			return Minimum(((const Node48 *)n)->ChildAt(idx));
		case NODE256:
			idx=0;
			while (!((const Node256 *)node)->ChildAt(idx)) ++idx;
			return Minimum(((const Node256 *)node)->ChildAt(idx));
		default:
			assert(0);
	}
}

} // namespace Mushroom

