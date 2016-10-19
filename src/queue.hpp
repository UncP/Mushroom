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

		virtual T Pop() = 0;

		virtual void Clear() = 0;

		virtual bool Empty() = 0;

		virtual ~Queue() { }

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
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(std::move(t));
			condition_.notify_one();
		}

		T Pop() override {
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

		void Clear() override {
			for (; !Empty();)
				std::this_thread::yield();
			std::unique_lock<std::mutex> lock(mutex_);
			assert(queue_.empty());
			clear_ = true;
			condition_.notify_all();
		}

		bool Empty() override {
			std::lock_guard<std::mutex> lock(mutex_);
			return queue_.empty();
		}

		~InfinityQueue() {
			if (!clear_)
				Clear();
			clear_ = true;
		}

	private:

		// using Queue<T>::queue_;
		using Queue<T>::mutex_;
		using Queue<T>::clear_;
		using Queue<T>::condition_;

		std::queue<T> queue_;
};

template<typename T>
class FiniteQueue : public Queue<T>
{
	public:
		FiniteQueue(int max = 4096):max_(max), front_(0), back_(0) {
			if (max_ > 4096) max_ = 4096;
			std::vector<T> tmp1(max_, T());
			queue_.swap(tmp1);
		}

		void Push(const T &t) override {
			std::unique_lock<std::mutex> lock(mutex_);
			int pre = front_++;
			for (; front_ == back_;)
				std::this_thread::yield();
			assert(front_ != back_);
			queue_[pre] = t;
			if (front_ == max_) front_ = 0;
			condition_.notify_one();
		}

		T Pop() override {
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [this]{ return front_ != back_ || clear_; });
			if (clear_) {
				assert(front_ == back_);
				return T();
			}
			assert(front_ != back_);
			T t(queue_[back_]);
			if (++back_ == max_) back_ = 0;
			return std::move(t);
		}

		void Clear() override {
			for (; !Empty();)
				std::this_thread::yield();
			std::lock_guard<std::mutex> lock(mutex_);
			assert(front_ == back_);
			clear_ = true;
			condition_.notify_all();
		}

		bool Empty() override {
			std::lock_guard<std::mutex> lock(mutex_);
			return front_ == back_;
		}

		~FiniteQueue() {
			if (!clear_)
				Clear();
			clear_ = true;
		}

	private:
		using Queue<T>::mutex_;
		using Queue<T>::clear_;
		using Queue<T>::condition_;

		std::vector<T>        queue_;
		int                   max_;
		int                   front_;
		int                   back_;
};

} // namespace Mushroom

#endif /* _QUEUE_HPP_ */