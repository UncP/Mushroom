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
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace Mushroom {

template<typename T>
class Queue
{
	public:
		Queue():clear_(false) { }

		virtual void Push(const T &t) = 0;

		virtual T Pull() = 0;

		virtual void Clear() = 0;

		virtual bool Empty() = 0;

		virtual bool Empty(int) = 0;

		virtual ~Queue() { }

	protected:
		std::mutex              mutex_;
		std::condition_variable condition_;
		bool                    clear_;
};

template<typename T>
class InfinityQueue : public Queue<T>
{
	public:
		InfinityQueue() { }

		void Push(const T &t) override {
			std::unique_lock<std::mutex> lock(mutex_);
			queue_.push(std::move(t));
			condition_.notify_one();
		}

		T Pull() override {
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [this]{ return !Empty(1) || clear_; });
			if (clear_) {
				assert(Empty(1));
				return T();
			}
			assert(!Empty(1));
			T t(std::move(queue_.front()));
			queue_.pop();
			return std::move(t);
		}

		void Clear() override {
			for (; !Empty();)
				std::this_thread::yield();
			std::unique_lock<std::mutex> lock(mutex_);
			assert(Empty(1));
			clear_ = true;
			condition_.notify_all();
		}

		bool Empty() override {
			std::lock_guard<std::mutex> lock(mutex_);
			return queue_.empty();
		}

		bool Empty(int) override {
			return queue_.empty();
		}

		~InfinityQueue() {
			if (!clear_)
				Clear();
			clear_ = true;
		}

	private:

		using Queue<T>::mutex_;
		using Queue<T>::clear_;
		using Queue<T>::condition_;

		std::queue<T> queue_;
};

template<typename T>
class FiniteQueue : public Queue<T>
{
	public:
		FiniteQueue(int capacity = 1024):capacity_(capacity), front_(0), back_(0) {
			if (capacity_ > 1024) capacity_ = 1024;
			std::vector<T> tmp1(capacity_, T());
			queue_.swap(tmp1);
		}

		void Push(const T &t) override {
			for (; Full();)
				std::this_thread::yield();
			std::unique_lock<std::mutex> lock(mutex_);
			queue_[front_] = t;
			if (++front_ == capacity_) front_ = 0;
			condition_.notify_one();
		}

		T Pull() override {
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [this]{ return !Empty(1) || clear_; });
			if (clear_) {
				assert(Empty(1));
				return T();
			}
			assert(!Empty(1));
			T t(queue_[back_]);
			if (++back_ == capacity_) back_ = 0;
			return std::move(t);
		}

		void Clear() override {
			for (; !Empty();)
				std::this_thread::yield();
			std::lock_guard<std::mutex> lock(mutex_);
			assert(Empty(1));
			clear_ = true;
			condition_.notify_all();
		}

		bool Empty() override {
			std::lock_guard<std::mutex> lock(mutex_);
			return front_ == back_;
		}

		bool Empty(int) override {
			return front_ == back_;
		}

		~FiniteQueue() {
			if (!clear_)
				Clear();
			clear_ = true;
		}

	private:

		bool Full() {
			std::lock_guard<std::mutex> lock(mutex_);
			return (front_ + 1) % capacity_ == back_;
		}

		using Queue<T>::mutex_;
		using Queue<T>::clear_;
		using Queue<T>::condition_;

		std::vector<T>        queue_;
		int                   capacity_;
		int                   front_;
		int                   back_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */