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

Queue::Queue(int capacity, uint8_t len):clear_(false), capacity_(capacity),
front_(0), avail_back_(0), work_back_(0)
{
	if (capacity <= 0 || capacity_ > 1024)
		capacity_ = 1024;
	queue_.reserve(capacity_);
	for (int i = 0; i != capacity_; ++i)
		queue_.push_back(new Task(len));
	std::vector<int> tmp1(capacity_);
	avail_.swap(tmp1);
	for (int i = 0; i != capacity_; ++i)
		avail_[i] = i;
	std::vector<int> tmp2(capacity_, -1);
	work_.swap(tmp2);
}

void Queue::Push(Status (BTree::*(fun))(KeySlice *), BTree *btree, KeySlice *key)
{
	std::unique_lock<std::mutex> lock(mutex_);
	empty_.wait(lock, [this]{ return avail_[front_] >= 0; });
	int seq = avail_[front_];
	Task *task = queue_[seq];
	task->Assign(fun, btree, key);
	avail_[front_] = -1;
	work_[front_++] = seq;
	if (front_ == capacity_)
		front_ = 0;
	ready_.notify_one();
}

void Queue::Pull()
{
	std::unique_lock<std::mutex> lock(mutex_);
	ready_.wait(lock, [this]{ return work_[work_back_] >= 0 || clear_; });
	if (clear_) return ;
	int seq = work_[work_back_];
	Task *task = queue_[seq];
	work_[work_back_++] = -1;
	if (work_back_ == capacity_) work_back_ = 0;
	lock.unlock();

	(*task)();

	lock.lock();
	avail_[avail_back_++] = seq;
	if (avail_back_ == capacity_) avail_back_ = 0;
	if (avail_back_ == work_back_)
		empty_.notify_one();
}

void Queue::Clear()
{
	std::unique_lock<std::mutex> lock(mutex_);
	empty_.wait(lock, [this]{ return front_ == avail_back_ && front_ == work_back_; });
	clear_ = true;
	ready_.notify_all();
}

Queue::~Queue()
{
	if (!clear_)
		Clear();
	std::for_each(queue_.begin(), queue_.end(), [](Task *task) {
		delete task;
	});
}

} // namespace Mushroom
