/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2016-10-16 18:51:28
**/

#ifndef _THREAD_POOL_MAPPING_HPP_
#define _THREAD_POOL_MAPPING_HPP_

#include "../include/utility.hpp"
#include "../include/thread.hpp"
#include "bounded_mapping_queue.hpp"

namespace Mushroom {

template<typename T>
class ThreadPoolMapping : private NoCopyTemplate<T>
{
	public:
		ThreadPoolMapping(BoundedMappingQueue<T> *queue, int thread_num);

		~ThreadPoolMapping();

		void Run();

		void Clear();

	private:
		bool                     working_;
		int                      thread_num_;
		BoundedMappingQueue<T>  *queue_;
		Thread                 **threads_;
};

template<typename T>
ThreadPoolMapping<T>::ThreadPoolMapping(BoundedMappingQueue<T> *queue, int thread_num)
:working_(false), thread_num_(thread_num), queue_(queue)
{
	if (thread_num_ <= 0)
		thread_num_ = 1;
	if (thread_num_ > 4)
		thread_num_ = 4;

	threads_ = new Thread*[thread_num_];

	working_ = true;

	for (int i = 0; i != thread_num_; ++i) {
		threads_[i] = new Thread([this] { this->Run(); });
		threads_[i]->Start();
	}
}

template<typename T>
void ThreadPoolMapping<T>::Run()
{
	for (;;) {
		int pos;
		T* task = queue_->Pop(&pos);

		if (!task) break;

		(*task)();

		queue_->Put(pos);
	}
}

template<typename T>
void ThreadPoolMapping<T>::Clear()
{
	if (!working_) return ;

	queue_->Clear();

	working_ = false;

	for (int i = 0; i != thread_num_; ++i)
		threads_[i]->Stop();
}

template<typename T>
ThreadPoolMapping<T>::~ThreadPoolMapping()
{
	Clear();

	for (int i = 0; i != thread_num_; ++i)
		delete threads_[i];

	delete [] threads_;
}

} // namespace Mushroom

#endif /* _THREAD_POOL_MAPPING_HPP_ */