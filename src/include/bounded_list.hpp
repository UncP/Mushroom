/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-14 16:09:39
**/

#ifndef _BOUNDED_LIST_HPP_
#define _BOUNDED_LIST_HPP_

#include <list>

#include "utility.hpp"
#include "mutex.hpp"
#include "cond.hpp"

namespace Mushroom {

template<typename T>
class BoundedList : private NoCopyTemplate<T>
{
	public:
		BoundedList(int capacity, const std::function<T*()> &constructor);

		~BoundedList();

		inline T* Get();

		inline void Push(T *t);

		inline T* Pop();

		inline void Put(T *);

		void Clear();

	private:
		bool           clear_;
		int            capacity_;
		std::list<T*>  free_;
		std::list<T*>  busy_;
		Mutex          free_mutex_;
		Mutex          busy_mutex_;
		Cond           free_cond_;
		Cond           busy_cond_;
};

template<typename T>
BoundedList<T>::BoundedList(int capacity, const std::function<T*()> &constructor)
:clear_(false), capacity_(capacity)
{
	if (capacity_ <= 0)
		capacity_ = 8;
	if (capacity_ > 1024)
		capacity_ = 1024;

	for (int i = 0; i < capacity_; ++i)
		free_.push_back(constructor());
}

template<typename T>
BoundedList<T>::~BoundedList()
{
	Clear();

	for (auto e : free_)
		delete e;
}

template<typename T>
void BoundedList<T>::Clear()
{
	free_mutex_.Lock();

	if (clear_) {
		free_mutex_.Unlock();
		return ;
	}

	while (int(free_.size()) != capacity_)
		free_cond_.Wait(free_mutex_);

	clear_ = true;

	free_mutex_.Unlock();
	busy_cond_.Broadcast();
}

template<typename T>
inline T* BoundedList<T>::Get()
{
	free_mutex_.Lock();
	while (free_.empty())
		free_cond_.Wait(free_mutex_);
	T* t = free_.front();
	free_.pop_front();
	free_mutex_.Unlock();
	return t;
}

template<typename T>
inline void BoundedList<T>::Push(T *t)
{
	busy_mutex_.Lock();
	busy_.push_back(t);
	busy_mutex_.Unlock();
	busy_cond_.Signal();
}

template<typename T>
inline T* BoundedList<T>::Pop()
{
	busy_mutex_.Lock();
	while (busy_.empty() && !clear_)
		busy_cond_.Wait(busy_mutex_);
	if (clear_) {
		busy_mutex_.Unlock();
		return 0;
	}
	T* t = busy_.front();
	busy_.pop_front();
	busy_mutex_.Unlock();
	return t;
}

template<typename T>
inline void BoundedList<T>::Put(T *t)
{
	free_mutex_.Lock();
	free_.push_back(t);
	free_mutex_.Unlock();
	free_cond_.Signal();
}

} // namespace Mushroom

#endif /* _BOUNDED_LIST_HPP_ */