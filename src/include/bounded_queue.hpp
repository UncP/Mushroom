/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-09 21:14:07
**/

#ifndef _BOUNDED_QUEUE_HPP_
#define _BOUNDED_QUEUE_HPP_

#include "utility.hpp"
#include "mutex.hpp"
#include "cond.hpp"

namespace Mushroom {

template<typename T>
class BoundedQueue : private NoCopyTemplate<T>
{
	public:
		BoundedQueue(int capacity);

		~BoundedQueue();

		inline void Push(T &&);

		inline T Pop();

		inline void Clear();

	private:
		bool   clear_;
		int    beg_;
		int    end_;
		int    capacity_;
		T     *queue_;
		Mutex  mutex_;
		Cond   empty_;
		Cond   full_;
};

template<typename T>
BoundedQueue<T>::BoundedQueue(int capacity):clear_(false), beg_(0), end_(0),capacity_(capacity)
{
	if (capacity_ <= 0)
		capacity_ = 8;
	if (capacity_ > 1024)
		capacity_ = 1024;

	queue_ = new T[capacity_];
}

template<typename T>
BoundedQueue<T>::~BoundedQueue()
{
	delete [] queue_;
}

template<typename T>
inline void BoundedQueue<T>::Clear()
{
	mutex_.Lock();
	if (clear_) {
		mutex_.Unlock();
		return ;
	}
	clear_ = true;
	empty_.Broadcast();
	mutex_.Unlock();
}

template<typename T>
inline void BoundedQueue<T>::Push(T &&t)
{
	mutex_.Lock();
	while (((end_+1)%capacity_) == beg_)
		full_.Wait(mutex_);
	queue_[end_] = t;
	if (++end_ == capacity_)
		end_ = 0;
	mutex_.Unlock();
	empty_.Signal();
}

template<typename T>
inline T BoundedQueue<T>::Pop()
{
	mutex_.Lock();
	while (beg_ == end_ && !clear_)
		empty_.Wait(mutex_);
	if (clear_) {
		mutex_.Unlock();
		return T();
	}
	T t = std::move(queue_[beg_]);
	if (++beg_ == capacity_)
		beg_ = 0;
	mutex_.Unlock();
	full_.Signal();
	return t;
}

} // namespace Mushroom

#endif /* _BOUNDED_QUEUE_HPP_ */