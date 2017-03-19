/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 19:02:14
**/

#include <iostream>

#include "thread_pool.hpp"

namespace Mushroom {

void Thread::Start()
{
	std::thread thread(func_);
	thread_.swap(thread);
}

void Thread::Stop()
{
	if (thread_.joinable())
		thread_.join();
}

ThreadPool::ThreadPool(Queue *queue):queue_(queue), working_(false)
{
	int thread_num = std::thread::hardware_concurrency();

	std::vector<std::shared_ptr<Thread>> threads(thread_num);
	threads_.swap(threads);

	for (auto &each : threads_) {
		each = ThreadPool::CreateThread([this]() { Run(); });
		each->Start();
	}

	working_ = true;
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
	for (auto e : threads_)
		e->Stop();
}

ThreadPool::~ThreadPool()
{
	if (working_)
		Clear();
	working_ = false;
	delete queue_;
}

} // namespace Mushroom
