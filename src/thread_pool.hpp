/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <thread>
#include <functional>
#include <vector>
#include <memory>

#include "task.hpp"
#include "queue.hpp"

namespace Mushroom {

class ThreadPool;

class Thread
{
	friend class ThreadPool;

	public:
		void Start();

		void Stop();

		auto Id() const { return thread_.get_id(); }

		Thread(const Thread &) = delete;
		Thread& operator=(const Thread &) = delete;

	private:

		Thread(const std::function<void()> &func):func_(func) { }

		std::function<void()>   func_;
		std::thread             thread_;
};

class ThreadPool
{
	public:

		static std::shared_ptr<Thread> CreateThread(const std::function<void()> &func) {
			return std::shared_ptr<Thread>(new Thread(func));
		}

		ThreadPool(Queue *queue);

		void AddTask(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key);

		void Run();

		void Clear();

		~ThreadPool();

	private:
		std::vector<std::shared_ptr<Thread>> threads_;
		Queue                               *queue_;
		bool                                 working_;
};

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */