/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 14:12:46
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include <queue>
#include <mutex>
#include <condition_variable>

#include "btree.hpp"
#include "slice.hpp"
#include "status.hpp"

namespace Mushroom {

class Task
{
	public:
		Task(Status (*fun)(const KeySlice *), BTree *btree, const KeySlice *key)
		:fun_(fun), btree_(btree), key_(key) { }

		Task(const Task &) = default;

		~Task() { delete key_; }

	private:
		Status        (*fun_)(const KeySlice *);
		BTree          *btree_;
		const KeySlice *key_;
};

class TaskQueue
{
	public:
		TaskQueue() { }

		void Push(const Task &task);

		Task Pop();

		void Clear();

		bool Empty();

	private:
		std::queue<Task>        queue_;
		std::mutex              mutex_;
		std::condition_variable condition_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */