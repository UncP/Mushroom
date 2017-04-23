/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 19:02:14
**/

#include <cassert>

#include "thread_pool.hpp"

namespace Mushroom {

static inline void* run(void *pool)
{
	((ThreadPool *)pool)->Run();
	return 0;
}

bool Thread::Start()
{
	return !pthread_create(&id_, 0, func_, pool_);
}

bool Thread::Stop()
{
	return !pthread_join(id_, 0);
}

ThreadPool::ThreadPool(Queue *queue):queue_(queue), working_(false)
{
	threads_ = new Thread*[thread_num];

	working_ = true;

	for (int i = 0; i != thread_num; ++i) {
		threads_[i] = new Thread(&run, this);
		assert(threads_[i]->Start());
	}
}

void ThreadPool::AddTask(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key)
{
	queue_->Push(fun, db, key);
}

void ThreadPool::Run()
{
	for (;;) {
		queue_->Pull();
		if (!working_) break;
	}
}

void ThreadPool::Clear()
{
	if (!working_) return ;
	queue_->Clear();
	working_ = false;

	for (int i = 0; i != thread_num; ++i)
		assert(threads_[i]->Stop());
}

ThreadPool::~ThreadPool()
{
	if (working_)
		Clear();
	working_ = false;
	for (int i = 0; i != thread_num; ++i)
		delete threads_[i];

	delete [] threads_;
	delete queue_;
}

} // namespace Mushroom
