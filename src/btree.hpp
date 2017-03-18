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

#include "status.hpp"
#include "slice.hpp"
#include "btree_page.hpp"

#include "latch_manager.hpp"

namespace Mushroom {

class BTree
{
	public:
		static const uint16_t MAX_KEY_LENGTH = 256;

		BTree(const int fd, const int key_len);

		Status Close();

		uint8_t KeyLen() const { return key_len_; }

		Status Put(KeySlice *key);

		Status Get(KeySlice *key) const;

		BTreePage* First(page_id *page_no, int level) const;

		bool Next(KeySlice *key, page_id *page_no, uint16_t *index) const;

		void Traverse(int level) const;

		bool KeyCheck(std::ifstream &in, int total) const;

		std::string ToString() const;

		BTree& operator=(const BTree &) = delete;
		BTree(const BTree &) = delete;

		~BTree();

	private:

		Latch* DescendToLeaf(const KeySlice *key, page_id *stack, uint8_t *depth) const;

		Status SplitRoot(Latch *latch);

		void Insert(Latch **latch, KeySlice *key);

		LatchManager *latch_manager_;

		BTreePage    *root_;

		uint8_t  key_len_;
		uint16_t degree_;
};

} // namespace Mushroom

#endif /* _BTREE_HPP_ */