/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 14:12:46
**/

#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include <cassert>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace Mushroom {

template<typename T>
class Queue
{
	public:
		Queue():clear_(false) { }

		void Push(const T &t);

		T Pop();

		void Clear();

		bool Empty();

		size_t Size();

		~Queue() {
			if (!clear_)
				Clear();
			clear_ = true;
		}

	private:
		std::queue<T>           queue_;
		std::mutex              mutex_;
		std::condition_variable condition_;
		bool                    clear_;
};

template<typename T>
void Queue<T>::Push(const T &t) {
	std::lock_guard<std::mutex> lock(mutex_);
	queue_.push(std::move(t));
	condition_.notify_one();
}

template<typename T>
T Queue<T>::Pop() {
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return !queue_.empty() || clear_; });
	if (clear_) {
		assert(queue_.empty());
		return T();
	}
	T t(std::move(queue_.front()));
	queue_.pop();
	return std::move(t);
}

template<typename T>
void Queue<T>::Clear() {
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return queue_.empty(); });
	clear_ = true;
	condition_.notify_all();
}

template<typename T>
bool Queue<T>::Empty() {
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}

template<typename T>
size_t Queue<T>::Size() {
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.size();
}

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */