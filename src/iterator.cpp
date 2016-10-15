/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-14 10:55:44
**/

#include "iterator.hpp"

namespace Mushroom {

Iterator::Iterator(const BTree *btree)
:btree_(btree), curr_(1) {
	char *buf = new char[BTree::MAX_KEY_LENGTH + sizeof(page_id)];
	assert(buf);
	key_ = (KeySlice *)buf;
}

bool Iterator::Seek(const char *key)
{
	size_t len = strlen(key);
	assert(len <= BTree::MAX_KEY_LENGTH);
	memcpy(key_->Data(), key, len);
	return btree_->Get(key_, &curr_);
}

bool Iterator::Begin()
{
	memset(key_->Data(), 0, BTree::MAX_KEY_LENGTH);
	btree_->Get(key_, &curr_);
	return true;
}

bool Iterator::Next()
{
	return btree_->Next(key_, &curr_);
}

} // namespace Mushroom
