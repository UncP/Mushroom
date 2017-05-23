/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-09 21:16:59
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include "utility.hpp"
#include "bounded_queue.hpp"
#include "thread.hpp"
#include "atomic.hpp"

namespace Mushroom {

template<typename T>
class ThreadPool : private NoCopyTemplate<T>
{
	public:
		ThreadPool(BoundedQueue<T> *queue, int thread_num);

		~ThreadPool();

		void Run();

		void Clear();

	private:
		atomic_8_t        working_;
		int               thread_num_;
		BoundedQueue<T>  *queue_;
		Thread          **threads_;
};

template<typename T>
ThreadPool<T>::ThreadPool(BoundedQueue<T> *queue, int thread_num)
:working_(0), thread_num_(thread_num), queue_(queue)
{
	if (thread_num_ <= 0)
		thread_num_ = 1;
	else if (thread_num_ > 8)
		thread_num_ = 8;

	threads_ = new Thread*[thread_num_];

	for (int i = 0; i != thread_num_; ++i) {
		threads_[i] = new Thread([this] { this->Run(); });
		threads_[i]->Start();
	}
	working_ = 1;
}

template<typename T>
void ThreadPool<T>::Run()
{
	for (;;) {
		T task = queue_->Pop();

		if (!working_.get())
			break;

		task();
	}
}

template<typename T>
void ThreadPool<T>::Clear()
{
	if (!working_.get())
		return ;

	working_ = 0;

	queue_->Clear();

	for (int i = 0; i != thread_num_; ++i)
		threads_[i]->Stop();
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
	for (int i = 0; i != thread_num_; ++i)
		delete threads_[i];

	delete [] threads_;
}

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */