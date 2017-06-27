/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 09:42:33
**/

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <cstdint>
#include <cassert>
#include <emmintrin.h>

namespace Mushroom {

enum NodeType { NODE4 = 0x0, NODE16, NODE48, NODE256, LEAF };

class Node
{
	public:

		inline NodeType Type() const { return (NodeType)type_; }

		inline bool Full() const {
			switch(type_) {
				case NODE4   : return Count() ==  4;
				case NODE16  : return Count() == 16;
				case NODE48  : return Count() == 48;
				case NODE256 : return Count() ==  0; // uint8_t [0, 256)
				default : assert(0);
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
		Node* Descend(uint8_t byte) {
			for (int i = 0; i < Count(); ++i)
				if (key_[i] == byte)
					return child_[i];
			return 0;
		}

	private:
		uint8_t key_[4];
		Node   *child_[4];
};

class Node16 : public Node
{
	public:
		Node* Descend(uint8_t byte) {
			__m128i cmp = _mm_cmpeq_epi8(_mm_set1_epi8(byte), _mm_loadu_si128((__m128i *)key_));
			int bit = _mm_movemask_epi8(cmp) & int((1 << Count()) - 1);
			if (bit) return child_[__builtin_ctz(bit)];
			else return 0;
		}

	private:
		uint8_t key_[16];
		Node   *child_[16];
};

class Node48 : public Node
{
	public:
		Node* Descend(uint8_t byte) {
			uint8_t idx = index_[byte];
			if (idx) return child_[idx];
			else return 0;
		}

	private:
		uint8_t index_[256];
		Node   *child_[48];
};

class Node256 : public Node
{
	public:
		Node* Descend(uint8_t byte) {
			return child_[byte];
		}

	private:
		Node *child_[256];
};

} // namespace Mushroom

#endif /* _NODE_HPP_ */