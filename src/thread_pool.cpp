/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-16 19:02:14
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

ThreadPool::ThreadPool():working_(false)
{
	size_t thread_num = std::thread::hardware_concurrency();
	assert(thread_num > 0);

	std::vector<std::shared_ptr<Thread>> threads(thread_num);
	threads_.swap(threads);
}

void ThreadPool::Init()
{
	if (working_) return ;

	for (auto &each : threads_) {
		each = ThreadPool::CreateThread([this]() { Run(); });
		each->Start();
	}
	working_ = true;
}

void ThreadPool::Run()
{
	for (;;) {

	}
}

void ThreadPool::Close()
{
	if (!working_) return ;

	working_ = false;
	for (auto e : threads_) {
		std::cout << e->Id() << std::endl;
		e->Stop();
	}
}

} // namespace Mushroom
