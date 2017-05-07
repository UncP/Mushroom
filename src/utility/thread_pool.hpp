/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include "../utility/bounded_queue.hpp"
#include "../utility/thread.hpp"

namespace Mushroom {

template<typename T>
class ThreadPool
{
	public:
		ThreadPool(BoundedQueue<T> *queue, int thread_num);

		~ThreadPool();

		void Run();

		void Clear();

	private:
		bool              working_;
		int               thread_num_;
		BoundedQueue<T>  *queue_;
		Thread          **threads_;
};

template<typename T>
ThreadPool<T>::ThreadPool(BoundedQueue<T> *queue, int thread_num)
:working_(false), thread_num_(thread_num), queue_(queue)
{
	if (thread_num_ <= 0 || thread_num_ > 4)
		thread_num_ = 4;

	threads_ = new Thread*[thread_num_];

	working_ = true;

	for (int i = 0; i != thread_num_; ++i) {
		threads_[i] = new Thread([this] { this->Run(); });
		threads_[i]->Start();
	}
}

template<typename T>
void ThreadPool<T>::Run()
{
	while (1) {
		int pos;
		T* task = queue_->Pop(&pos);

		if (!task) break;

		(*task)();

		queue_->Put(pos);
	}
}

template<typename T>
void ThreadPool<T>::Clear()
{
	if (!working_)
		return ;

	queue_->Clear();

	working_ = false;

	for (int i = 0; i != thread_num_; ++i)
		threads_[i]->Stop();
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
	Clear();

	for (int i = 0; i != thread_num_; ++i)
		delete threads_[i];

	delete [] threads_;
}

} // namespace Mushroom

#endif /* _THREAD_POOL_HPP_ */