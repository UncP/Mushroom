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
		Task(uint8_t key_len);

		void Assign(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key);

		Status operator()() { return (btree_->*fun_)(key_); }

		Task(const Task &) = delete;
		Task(const Task &&) = delete;
		Task& operator=(const Task &) = delete;
		Task& operator=(const Task &&) = delete;

		~Task();

	private:
		Status         (BTree::*(fun_))(KeySlice *);
		BTree          *btree_;
		KeySlice       *key_;
		uint8_t         key_len_;
};

} // namespace Mushroom

#endif /* _TASK_HPP_ */