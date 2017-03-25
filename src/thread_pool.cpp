/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 19:02:14
**/

#include "thread_pool.hpp"

namespace Mushroom {

void Thread::Start()
{
	std::thread thread(func_);
	thread_.swap(thread);
}

void Thread::Stop()
{
	thread_.join();
}

ThreadPool::ThreadPool(Queue *queue):queue_(queue), working_(false)
{
	threads_ = new Thread*[thread_num];

	working_ = true;

	for (int i = 0; i != thread_num; ++i) {
		threads_[i] = ThreadPool::CreateThread([this]() { Run(); });
		threads_[i]->Start();
	}
}

void ThreadPool::AddTask(bool (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key)
{
	queue_->Push(fun, btree, key);
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
		threads_[i]->Stop();
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
