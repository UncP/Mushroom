/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 09:42:33
**/

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <emmintrin.h>
#include <cstdint>
#include <new>
#include <algorithm>

namespace Mushroom {

enum NodeType { NODE4 = 0x0, NODE16, NODE48, NODE256, LEAF };

#define IS_LEAF(x)  (uintptr_t(x) & 1)
#define SET_LEAF(x) (void *)(uintptr_t(x) | 1)
#define LEAF_RAW(x) (Leaf *)(uintptr_t(x) & ~1)

class Leaf
{
	public:
		Leaf(const uint8_t *key, uint32_t len, uint32_t val):val_(val), len_(len) {
			memcpy(key_, key, len_);
		}

		inline bool Match(const uint8_t *key, uint32_t len) const {
			if (len != len_) return false;
			return !memcmp(key_, key, len_);
		}

		uint32_t CommonPrefix(uint32_t depth, const Leaf *that) const {
			int max_cmp = std::min(this->len_, that->len_) - depth;
			int idx;
			for (idx = 0; idx < max_cmp; ++idx)
				if (this->key_[depth + idx] != that->key_[depth + idx])
					return idx;
			return idx;
		}

		inline uint32_t KeyLen() const { return len_; }

		inline uint8_t KeyAt(uint32_t idx) const {
			return key_[idx];
		}

		inline uint32_t Value() const { return val_; }

		inline const uint8_t* Key() const { return key_; }

	private:
		uint32_t val_;
		uint32_t len_;
		uint8_t  key_[0];
};

inline Leaf* NewLeaf(const uint8_t *key, uint32_t len, uint32_t val) {
	return new (new char[sizeof(Leaf) + len]) Leaf(key, len, val);
}

inline void DeleteLeaf(Leaf *leaf) {
	delete [] (char *)leaf;
}

class Node
{
	public:
		Node(NodeType type):type_(type), count_(0), len_(0) { }

		Node(NodeType type, uint8_t count, uint32_t len, const uint8_t *prefix)
		:type_(type), count_(count), len_(len) {
			memcpy(prefix_, prefix, std::min(MAX_PREFIX_LEN, len_));
		}

		inline NodeType Type() const { return (NodeType)type_; }

		inline uint8_t Count() const { return count_; }

		inline void SetPrefix(const uint8_t *prefix, uint32_t len) {
			len_ = len;
			memcpy(prefix_, prefix, std::min(MAX_PREFIX_LEN, len_));
		}

		inline uint32_t CheckPrefix(const uint8_t *key, uint32_t len, uint32_t depth) {
			int max_cmp = std::min(int(std::min(len_, MAX_PREFIX_LEN)), int(len - depth));
			int idx;
			for (idx = 0; idx < max_cmp; ++idx)
				if (prefix_[idx] != key[depth + idx])
					return idx;
			return idx;
		}

		inline void AdjustPrefix(uint32_t len) {
			len_ -= len + 1;
			memmove(prefix_, prefix_ + len + 1, std::min(MAX_PREFIX_LEN, len_));
		}

		inline void AdjustPrefix(uint32_t len, const uint8_t *key) {
			len_ -= len + 1;
			memcpy(prefix_, key + len + 1, std::min(MAX_PREFIX_LEN, len_));
		}

		inline const uint8_t* Prefix() const { return prefix_; }

		inline uint32_t PrefixLen() const { return len_; }

		static uint32_t MAX_PREFIX_LEN;

		uint32_t MismatchPrefix(const uint8_t *key, uint32_t len, uint32_t depth) {
			int max_cmp = std::min(int(std::min(MAX_PREFIX_LEN, len_)), int(len - depth));
			int idx;
			for (idx = 0; idx < max_cmp; ++idx) {
				if (prefix_[idx] != key[depth + idx])
					return idx;
			}

			if (len_ > MAX_PREFIX_LEN) {
				Leaf *leaf = Minimum(this);
				max_cmp = std::min(leaf->KeyLen(), len) - depth;
				for (; idx < max_cmp; ++idx) {
					if (leaf->KeyAt(depth + idx) != key[depth + idx])
						return idx;
				}
			}
			return idx;
		}

		Node(const Node &) = delete;
		Node& operator=(const Node &) = delete;

		static Leaf* Minimum(const Node *node);

	protected:
		inline void IncrCount() { ++count_; }

	private:
		uint8_t  type_;
		uint8_t  count_;
		uint32_t len_;
		uint8_t  prefix_[8];
};

class Node4 : public Node
{
	public:
		Node4():Node(NODE4) { memset(key_, 0, 4); }

		inline bool Full() const { return Count() == 4; }

		inline const uint8_t* Key() const { return key_; }

		inline const Node* Child() const { return (const Node *)child_; }

		inline Node* ChildAt(uint32_t idx) const {
			return child_[idx];
		}

		Node** Descend(uint8_t byte) {
			for (int i = 0; i < Count(); ++i)
				if (key_[i] == byte)
					return &child_[i];
			return 0;
		}

		void AddChild(uint8_t byte, void *child) {
			int idx;
			for (idx = 0; idx < Count(); ++idx)
				if (byte < key_[idx])
					break;
			memmove(key_ + idx + 1, key_ + idx, Count() - idx);
			memmove(child_ + idx + 1, child_ + idx, sizeof(Node *) * (Count() - idx));
			key_[idx] = byte;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t key_[4];
		Node   *child_[4];
};

class Node16 : public Node
{
	public:
		Node16(const Node4 *node4)
		:Node(NODE16, node4->Count(), node4->PrefixLen(), node4->Prefix()) {
			memset(key_, 0, 16);
			memcpy(key_, node4->Key(), node4->Count());
			memcpy(child_, node4->Child(), sizeof(Node *) * node4->Count());
		}

		inline bool Full() const { return Count() == 16; }

		inline const Node* Child() const { return (const Node *)child_; }

		inline uint8_t KeyAt(uint32_t idx) const {
			return key_[idx];
		}

		inline Node* ChildAt(uint32_t idx) const {
			return child_[idx];
		}

		Node** Descend(uint8_t byte) {
			__m128i cmp = _mm_cmpeq_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & ((1 << Count()) - 1);
			if (bit) return &child_[__builtin_ctz(bit)];
			else return 0;
		}

		void AddChild(uint8_t byte, void *child) {
			__m128i cmp = _mm_cmplt_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & ((1 << Count()) - 1);
			int idx;
			if (bit) {
				idx = __builtin_ctz(bit);
				memmove(key_ + idx + 1, key_ + idx, Count() - idx);
				memmove(child_ + idx + 1, child_ + idx, sizeof(Node *) * (Count() - idx));
			} else {
				idx = Count();
			}
			key_[idx] = byte;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t key_[16];
		Node   *child_[16];
};

class Node48 : public Node
{
	public:
		Node48(const Node16 *node16)
		:Node(NODE48, node16->Count(), node16->PrefixLen(), node16->Prefix()) {
			memset(index_, 0, 256); memset(child_, 0, sizeof(Node *) * 48);
			memcpy(child_, node16->Child(), sizeof(Node *) * node16->Count());
			for (uint8_t i = 0; i < node16->Count(); ++i)
				index_[node16->KeyAt(i)] = i + 1;
		}

		inline bool Full() const { return Count() == 48; }

		inline uint8_t KeyAt(uint32_t idx) const {
			return index_[idx];
		}

		inline Node* ChildAt(uint32_t idx) const {
			return child_[idx];
		}

		Node** Descend(uint8_t byte) {
			uint8_t idx = index_[byte];
			if (idx)
				return &child_[idx - 1];
			return 0;
		}

		void AddChild(uint8_t byte, void *child) {
			int idx = 0;
			while (child_[idx]) ++idx;
			index_[byte] = idx + 1;
			child_[idx] = (Node *)child;
			IncrCount();
		}

	private:
		uint8_t index_[256];
		Node   *child_[48];
};

class Node256 : public Node
{
	public:
		Node256(const Node48 *node48)
		:Node(NODE256, node48->Count(), node48->PrefixLen(), node48->Prefix()) {
			memset(child_, 0, sizeof(Node *) * 256);
			for (uint32_t i = 0; i < 256; ++i)
				if (node48->KeyAt(i))
					child_[i] = node48->ChildAt(node48->KeyAt(i) - 1);
		}

		inline Node* ChildAt(uint32_t idx) const {
			return child_[idx];
		}

		Node** Descend(uint8_t byte) {
			return &child_[byte];
		}

		void AddChild(uint8_t byte, void *child) {
			child_[byte] = (Node *)child;
			IncrCount();
		}

	private:
		Node *child_[256];
};

void Free(Node *node);

} // namespace Mushroom

#endif /* _NODE_HPP_ */