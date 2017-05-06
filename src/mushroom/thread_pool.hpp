/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include "utility.hpp"

namespace Mushroom {

class MushroomDB;
class KeySlice;
class Queue;
template<typename T> class Thread;

class ThreadPool
{
	public:
		ThreadPool(Queue *queue);

		void AddTask(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key);

		void Run();

		void Clear();

		~ThreadPool();

		ThreadPool(const ThreadPool &) = delete;
		ThreadPool& operator=(const ThreadPool &) = delete;

	private:
		static const int thread_num = 4;

		Thread<ThreadPool> **threads_;
		Queue               *queue_;
		bool                 working_;
};

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */