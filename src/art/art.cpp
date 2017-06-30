/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 10:40:44
**/

#include "art.hpp"

namespace Mushroom {

ART::ART():root_(0) { }

bool ART::Put(const uint8_t *key, uint32_t len, uint32_t val)
{
	return Insert(root_, &root_, key, 0, len, val);
}

bool ART::Insert(Node *cur, Node **ref, const uint8_t *key, uint32_t depth, uint32_t len,
	uint32_t val)
{
	if (!cur) {
		*ref = (Node *)SET_LEAF(NewLeaf(key, len, val));
		return true;
	}

	if (IsLeaf(cur)) {
		Leaf *leaf = LEAF_RAW(cur);
		if (leaf->Match(key, len)) return ;

		Leaf *leaf2 = NewLeaf(key, len, val);
		int prefix_len = leaf->CommonPrefix(leaf2, depth);

		Node4 *node4 = new Node4();
		node4->SetPrefix(key + depth, prefix_len);

		node4->AddChild(leaf->KeyAt(depth + prefix_len), SET_LEAF(leaf));
		node4->AddChild(leaf2->KeyAt(depth + prefix_len), SET_LEAF(leaf));

		*ref = (Node *)node4;
	}

	if (cur->PrefixLen()) {
		int prefix_diff = cur->MismatchPrefix(key, len, depth);
		if (uint32_t(prefix_diff) >= cur->PrefixLen()) {
			depth += cur->PrefixLen();
			goto Recurse_Search;
		}

		Node4 *node4 = new Node4();
		*ref = (Node *)node4;
		node4->SetPrefix(cur->Prefix(), prefix_diff);
	}
}

bool ART::Get(const uint8_t *key, uint32_t len, uint32_t *val)
{

	return false;
}

Node** ART::Descend(Node *cur, char byte)
{
	union {
		Node4   *p4;
		Node16  *p16;
		Node48  *p48;
		Node256 *p256;
	}p;
	switch (cur->Type()) {
		case NODE4   : p.p4   = (Node4   *)cur; return p.p4  ->Descend(byte); break;
		case NODE16  : p.p16  = (Node16  *)cur; return p.p16 ->Descend(byte); break;
		case NODE48  : p.p48  = (Node48  *)cur; return p.p48 ->Descend(byte); break;
		case NODE256 : p.p256 = (Node256 *)cur; return p.p256->Descend(byte); break;
		default : assert(0);
	}
}

} // namespace Mushroom
