/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time: 2016-10-18 15:30:39
**/

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "utility.hpp"
#include "slice.hpp"

namespace Mushroom {

class Task
{
	public:
		Task(uint8_t key_len);

		void Assign(bool (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key);

		bool operator()() { return (btree_->*fun_)(key_); }

		Task(const Task &) = delete;
		Task(const Task &&) = delete;
		Task& operator=(const Task &) = delete;
		Task& operator=(const Task &&) = delete;

		~Task();

	private:
		bool         (BTree::*(fun_))(KeySlice *);
		BTree        *btree_;
		KeySlice     *key_;
		uint8_t       key_len_;
};

} // namespace Mushroom

#endif /* _TASK_HPP_ */