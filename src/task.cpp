/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-11-29 15:25:34
**/

#include "task.hpp"

namespace Mushroom {

Task::Task(uint8_t key_len):fun_(nullptr), btree_(nullptr), key_(nullptr), key_len_(key_len)
{
	char *buf = new char[key_len_ + sizeof(page_id)];
	key_ = (KeySlice *)buf;
}

void Task::Assign(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key)
{
	fun_   = fun;
	btree_ = btree;
	CopyKey(key_, key, 0, key_len_);
}

Task::~Task()
{
	 delete [] key_;
}

} // namespace Mushroom
