/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 10:40:44
**/

#include "art.hpp"

namespace Mushroom {

ART::ART():root_(0) { }

static Node** Descend(Node *cur, char byte)
{
	union {
		Node4   *p4;
		Node16  *p16;
		Node48  *p48;
		Node256 *p256;
	}p;
	switch(cur->Type()) {
		case NODE4   : p.p4   = (Node4   *)cur; return p.p4  ->Descend(byte);
		case NODE16  : p.p16  = (Node16  *)cur; return p.p16 ->Descend(byte);
		case NODE48  : p.p48  = (Node48  *)cur; return p.p48 ->Descend(byte);
		case NODE256 : p.p256 = (Node256 *)cur; return p.p256->Descend(byte);
		default : assert(0);
	}
}

static void AddChild(Node *cur, Node **ref, uint8_t byte, void *child)
{
	union {
		Node4   *p4;
		Node16  *p16;
		Node48  *p48;
		Node256 *p256;
	}p;
	switch(cur->Type()) {
		case NODE4:
			p.p4 = (Node4 *)cur;
			if (p.p4->Full()) {
				Node16 *node16 = new Node16(p.p4);
				*ref = node16;
				delete cur;
				node16->AddChild(byte, child);
			} else {
				p.p4->AddChild(byte, child);
			}
			break;
		case NODE16:
			p.p16  = (Node16 *)cur;
			if (p.p16->Full()) {
				Node48 *node48 = new Node48(p.p16);
				*ref = node48;
				delete cur;
				node48->AddChild(byte, child);
			} else {
				p.p16->AddChild(byte, child);
			}
			break;
		case NODE48:
			p.p48  = (Node48 *)cur;
			if (p.p48->Full()) {
				Node256 *node256 = new Node256(p.p48);
				*ref = node256;
				delete cur;
				node256->AddChild(byte, child);
			} else {
				p.p48->AddChild(byte, child);
			}
			break;
		case NODE256:
			p.p256 = (Node256 *)cur;
			p.p256->AddChild(byte, child);
			break;
		default:
			assert(0);
	}
}

static bool Insert(Node *cur, Node **ref, const uint8_t *key, uint32_t depth, uint32_t len,
	uint32_t val)
{
	if (!cur) {
		*ref = (Node *)SET_LEAF(NewLeaf(key, len, val));
		return true;
	}

	if (IS_LEAF(cur)) {
		Leaf *leaf = LEAF_RAW(cur);
		if (leaf->Match(key, len)) return false;

		Leaf *leaf2 = NewLeaf(key, len, val);
		uint32_t prefix_len = leaf->CommonPrefix(depth, leaf2);

		Node4 *node4 = new Node4();
		*ref = (Node *)node4;

		node4->SetPrefix(key + depth, prefix_len);

		node4->AddChild(leaf->KeyAt(depth + prefix_len), SET_LEAF(leaf));
		node4->AddChild(leaf2->KeyAt(depth + prefix_len), SET_LEAF(leaf2));
		return true;
	}

	if (cur->PrefixLen()) {
		uint32_t prefix_diff = cur->MismatchPrefix(key, len, depth);
		if (prefix_diff >= cur->PrefixLen()) {
			depth += cur->PrefixLen();
			goto Recurse_Search;
		}

		Node4 *node4 = new Node4();
		*ref = (Node *)node4;
		node4->SetPrefix(cur->Prefix(), prefix_diff);

		if (node4->PrefixLen() <= Node::MAX_PREFIX_LEN) {
			node4->AddChild(cur->Prefix()[prefix_diff], cur);
			cur->AdjustPrefix(prefix_diff);
		} else {
			Leaf *leaf = Node::Minimum(cur);
			node4->AddChild(leaf->KeyAt(depth + prefix_diff), cur);
			cur->AdjustPrefix(prefix_diff, depth, leaf->Key());
		}

		Leaf *leaf = NewLeaf(key, len, val);
		node4->AddChild(key[depth + prefix_diff], SET_LEAF(leaf));
		return true;
	}

Recurse_Search:
	Node **child = Descend(cur, key[depth]);
	if (child)
		return Insert(*child, child, key, depth + 1, len, val);

	Leaf *leaf = NewLeaf(key, len, val);
	AddChild(cur, ref, key[depth], SET_LEAF(leaf));
	return true;
}

bool ART::Put(const uint8_t *key, uint32_t len, uint32_t val)
{
	return Insert(root_, &root_, key, 0, len, val);
}

bool ART::Get(const uint8_t *key, uint32_t len, uint32_t *val)
{
	Node *cur = root_;
	uint32_t depth = 0;
	for (; cur) {
		if (IS_LEAF(cur)) {
			Leaf *leaf = LEAF_RAW(cur);
			if (leaf->Match(key, len)) {
				*val = leaf->Value();
				return true;
			}
			return false;
		}

		if (cur->PrefixLen()) {
			uint32_t prefix_len = cur->CheckPrefix(key, len, depth);
			if (prefix_len != std::min(Node::MAX_PREFIX_LEN, cur->PrefixLen()))
				return false;
			depth += cur->PrefixLen();
		}

		Node **child = Descend(cur, key[depth]);
		cur = child ? *child : 0;
		++depth;
	}
	return false;
}

} // namespace Mushroom
