/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <memory>

#include "task_queue.hpp"

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

		ThreadPool();

		void Init();

		void Run();

		void Close();

		~ThreadPool() {
			if (working_)
				Close();
			working_ = false;
		}

	private:
		std::vector<std::shared_ptr<Thread>> threads_;
		TaskQueue                            queue_;
		bool                                 working_;
};

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */