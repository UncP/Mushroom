/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-07 20:10:30
**/

#ifndef _BTREE_HPP_
#define _BTREE_HPP_

#include <fstream>
#include <string>

#include "utility.hpp"
#include "slice.hpp"
#include "latch_manager.hpp"
#include "pool_manager.hpp"

namespace Mushroom {

class BTree
{
	public:
		static const uint32_t MAX_KEY_LENGTH = 256;

		BTree(int key_len, LatchManager *latch_manager, PoolManager *page_manager, page_id *root);

		void Initialize();

		bool Free();

		bool Put(KeySlice *key);

		bool Get(KeySlice *key) const;

		BTreePage* First(page_id *page_no, int level) const;

		bool Next(KeySlice *key, page_id *page_no, uint16_t *index) const;

		bool KeyCheck(std::ifstream &in, int total) const;

		BTree(const BTree &) = delete;
		BTree(const BTree &&) = delete;
		BTree& operator=(const BTree &) = delete;
		BTree& operator=(const BTree &&) = delete;

		~BTree();

	private:
		struct Set {
			Set():depth_(0) { }
			page_id    page_no_;
			Latch     *latch_;
			BTreePage *page_;
			page_id    stack_[8];
			uint8_t    depth_;
		};

		void DescendToLeaf(const KeySlice *key, Set &set) const;

		void SplitRoot(Set &set);

		void Insert(Set &set, KeySlice *key);

		LatchManager *latch_manager_;
		PoolManager  *page_manager_;

		page_id      *root_;

		uint8_t       key_len_;
		uint16_t      degree_;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */