/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 09:42:33
**/

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <cstdint>
#include <cstring>
#include <cassert>
#include <emmintrin.h>

namespace Mushroom {

enum NodeType { NODE4 = 0x0, NODE16, NODE48, NODE256, LEAF };

class Node
{
	public:
		Node():count_(0), len_(0) { }
		inline NodeType Type() const { return (NodeType)type_; }

		inline bool Full() const {
			switch(type_) {
				case NODE4   : return Count() ==  4;
				case NODE16  : return Count() == 16;
				case NODE48  : return Count() == 48;
				case NODE256 : return Count() ==  0; // uint8_t [0, 256)
				default: assert(0);
			};
		}

		inline uint8_t Count() const { return count_; }

		Node(const Node &) = delete;
		Node& operator=(const Node &) = delete;

	private:
		static const uint32_t MAX_PRE_LEN = 8;

		uint8_t type_;
		uint8_t count_;
		uint8_t len_;
		uint8_t prefix_[MAX_PRE_LEN];
};

class Node4 : public Node
{
	public:
		Node4():Node(), type_(NODE4) { }

		Node** Descend(uint8_t byte) {
			for (int i = 0; i < Count(); ++i)
				if (key_[i] == byte)
					return &child_[i];
			return 0;
		}

		void Add(uint8_t byte, void *child) {
			int idx;
			for (idx = 0; idx < Count(); ++idx)
				if (key_[idx] > byte)
					break;
			memmove(key_ + idx + 1, key_ + idx, Count() - idx);
			key_[idx] = byte;
			child_[idx] = leaf;
		}

	private:
		uint8_t key_[4];
		Node   *child_[4];
};

class Node16 : public Node
{
	public:
		Node16():Node(), type_(NODE16) { }

		Node** Descend(uint8_t byte) {
			__m128i cmp = _mm_cmpeq_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & ((1 << Count()) - 1);
			if (bit) return &child_[__builtin_ctz(bit)];
			else return 0;
		}

		void Add(uint8_t byte, void *child) {
			int mask = (1 << Count()) - 1;
		}

	private:
		uint8_t key_[16];
		Node   *child_[16];
};

class Node48 : public Node
{
	public:
		Node48():Node(), type_(NODE48) { }

		Node** Descend(uint8_t byte) {
			uint8_t idx = index_[byte];
			if (idx) return &child_[idx];
			else return 0;
		}

	private:
		uint8_t index_[256];
		Node   *child_[48];
};

class Node256 : public Node
{
	public:
		Node256():Node(), type_(NODE256) { }

		Node** Descend(uint8_t byte) {
			return &child_[byte];
		}

	private:
		Node *child_[256];
};

class Leaf
{
	public:
		Leaf(const uint8_t *key, uint32_t len, uint32_t val):val_(val), len_(len) {
			memcpy(key_, key, len_);
		}

	private:
		uint32_t val_;
		uint32_t len_;
		uint8_t  key_[0];
};

inline Leaf* NewLeaf(const uint8_t *key, uint32_t len, uint32_t val) {
	return new (new char[8 + len]) Leaf(key, len, val);
}

inline void DeleteLeaf(Leaf *leaf) {
	delete [] (char *)leaf;
}

} // namespace Mushroom

#endif /* _NODE_HPP_ */