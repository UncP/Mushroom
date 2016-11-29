/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-17 14:21:54
**/

#include "queue.hpp"

namespace Mushroom {

Queue::Queue(int capacity, uint8_t len):clear_(false), capacity_(capacity), front_(0), back_(0)
{
	if (capacity <= 0 || capacity_ > 1024) capacity_ = 1024;
	for (int i = 0; i != capacity_; ++i)
		queue_.push_back(new Task(len));
}

void Queue::Push(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key)
{
	for (; Full();)
		std::this_thread::yield();
	std::unique_lock<std::mutex> lock(mutex_);
	queue_[front_]->Assign(fun, btree, key);
	if (++front_ == capacity_) front_ = 0;
	condition_.notify_one();
}

Task* Queue::Pull()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return !Empty(1) || clear_; });
	if (clear_) return nullptr;
	Task *task = queue_[back_];
	if (++back_ == capacity_) back_ = 0;
	return task;
}

void Queue::Clear()
{
	for (; !Empty();)
		std::this_thread::yield();
	std::lock_guard<std::mutex> lock(mutex_);
	clear_ = true;
	condition_.notify_all();
}

bool Queue::Full()
{
	std::lock_guard<std::mutex> lock(mutex_);
	return (front_ + 1) % capacity_ == back_;
}

bool Queue::Empty(int)
{
	return front_ == back_;
}

bool Queue::Empty()
{
	std::lock_guard<std::mutex> lock(mutex_);
	return front_ == back_;
}

Queue::~Queue()
{
	if (!clear_)
		Clear();
	clear_ = true;
	std::for_each(queue_.begin(), queue_.end(), [](Task *task) {
		delete task;
	});
}

} // namespace Mushroom
