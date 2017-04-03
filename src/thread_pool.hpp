/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <pthread.h>

#include "utility.hpp"
#include "queue.hpp"

namespace Mushroom {

class Thread
{
	friend class ThreadPool;
	public:
		bool Start();

		bool Stop();

		Thread(const Thread &) = delete;
		Thread& operator=(const Thread &) = delete;

	private:
		Thread(void* (*func)(void *), ThreadPool *pool):func_(func), pool_(pool) { }

		void* (*func_)(void *);
		ThreadPool *pool_;
		pthread_t id_;
};

class ThreadPool
{
	public:
		static Thread* CreateThread(void* (*func)(void *), ThreadPool *pool) {
			return new Thread(func, pool);
		}

		ThreadPool(Queue *queue);

		void AddTask(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key);

		void Run();

		void Clear();

		~ThreadPool();

	private:
		static const int thread_num = 4;

		Thread **threads_;
		Queue   *queue_;
		bool     working_;
};

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */