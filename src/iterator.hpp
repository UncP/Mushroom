/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-14 10:41:39
**/

#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_

#include <cassert>

#include "slice.hpp"
#include "btree.hpp"

namespace Mushroom {

class Iterator
{
	public:
		Iterator(const BTree *btree):btree_(btree), curr_(0) {
			char *buf = new char[BTree::MAX_KEY_LENGTH + sizeof(page_id)];
			assert(buf);
			key_ = (KeySlice *)buf;
		}

		bool Begin();

		bool Seek(const char *key);

		bool Next();

		const KeySlice* Key() const { return key_; }

		~Iterator() { if (key_) delete [] key_; key_ = nullptr; }

	private:
		const BTree *btree_;
		page_id      curr_;
		KeySlice    *key_;
};

} // namespace Mushroom

#endif /* _ITERATOR_HPP_ */