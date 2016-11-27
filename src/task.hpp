/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-18 15:30:39
**/

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "btree.hpp"
#include "slice.hpp"
#include "status.hpp"

namespace Mushroom {

class Task
{
	public:
		Task():fun_(nullptr), btree_(nullptr), key_(nullptr) { }

		Task(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key)
		:fun_(fun), btree_(btree), key_(key) { }

		Status operator()() {
			if (fun_)
				return (btree_->*fun_)(key_);
			else
				return Success;
		}

	private:
		Status         (BTree::*(fun_))(KeySlice *);
		BTree          *btree_;
		KeySlice       *key_;
};

} // namespace Mushroom

#endif /* _TASK_HPP_ */