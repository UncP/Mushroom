/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-14 10:41:39
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
		Iterator(const BTree *btree, int level = 0);

		bool Begin();

		bool Seek(const char *key);

		bool Next();

		bool CheckBtree();

		const KeySlice* Key() const { return key_; }

		~Iterator() { delete [] key_; }

	private:
		const BTree *btree_;
		int          level_;
		page_id      curr_;
		uint16_t     index_;
		KeySlice    *key_;
};

} // namespace Mushroom

#endif /* _ITERATOR_HPP_ */