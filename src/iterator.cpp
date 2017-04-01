/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-14 10:55:44
**/

#include <cassert>

#include "iterator.hpp"
#include "slice.hpp"
#include "blinktree.hpp"

namespace Mushroom {

Iterator::Iterator(const BLinkTree *btree, int level)
:btree_(btree), level_(level), curr_(0), index_(0) {
	char *buf = new char[BLinkTree::MAX_KEY_LENGTH + sizeof(page_id)];
	assert(buf);
	key_ = (KeySlice *)buf;
}

Iterator::~Iterator()
{
	delete [] key_;
}

bool Iterator::Seek(const char *key)
{
	size_t len = strlen(key);
	if (len > BLinkTree::MAX_KEY_LENGTH) return false;
	memcpy(key_->Data(), key, len);
	return btree_->Get(key_);
}

bool Iterator::Begin()
{
	memset(key_->Data(), 0, BLinkTree::MAX_KEY_LENGTH);
	if (btree_->First(&curr_, level_))
		return true;
	return false;
}

bool Iterator::Next()
{
	return btree_->Next(key_, &curr_, &index_);
}

bool Iterator::CheckBtree()
{
	uint32_t key_len = BLinkTree::MAX_KEY_LENGTH;

	char buf[BLinkTree::MAX_KEY_LENGTH + sizeof(page_id)] = { 0 };
	KeySlice *pre = (KeySlice *)buf;

	for (level_ = 0; ; ++level_) {
		int count = 0;
		if (!Begin()) break;
		memset(pre, 0, sizeof(buf));
		for (; Next();) {
			++count;
			if (CompareSuffix(pre, key_, 0, key_len) >= 0) {
				return false;
			}
			memcpy(pre, key_, sizeof(page_id) + key_len);
		}
		printf("\nlevel: %d  total: %d\n", level_, count);
	}
	return true;
}

} // namespace Mushroom
