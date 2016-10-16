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

Iterator::Iterator(const BTree *btree, int level)
:btree_(btree), level_(level), curr_(0), index_(0) {
	char *buf = new char[BTree::MAX_KEY_LENGTH + sizeof(page_id)];
	assert(buf);
	key_ = (KeySlice *)buf;
}

bool Iterator::Seek(const char *key)
{
	size_t len = strlen(key);
	if (len > BTree::MAX_KEY_LENGTH) return false;
	memcpy(key_->Data(), key, len);
	return btree_->Get(key_);
}

bool Iterator::Begin()
{
	memset(key_->Data(), 0, BTree::MAX_KEY_LENGTH);
	if (btree_->First(&curr_, level_))
		return true;
	return false;
}

bool Iterator::Next()
{
	return btree_->Next(key_, &curr_, &index_);
}

} // namespace Mushroom
