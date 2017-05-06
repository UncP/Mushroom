/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-17 14:21:54
**/

#include "queue.hpp"
#include "task.hpp"

namespace Mushroom {

Queue::Queue(int capacity, uint8_t len):clear_(false), capacity_(capacity),
front_(0), avail_back_(0), work_back_(0)
{
	if (capacity <= 0 || capacity_ > 1024)
		capacity_ = 1024;

	queue_ = new Task*[capacity_];
	for (int i = 0; i != capacity_; ++i)
		queue_[i] = new Task(len);

	avail_ = new int[capacity_];
	for (int i = 0; i != capacity_; ++i)
		avail_[i] = i;

	work_ = new int[capacity_];
	for (int i = 0; i != capacity_; ++i)
		work_[i] = -1;
}

Queue::~Queue()
{
	if (!clear_)
		Clear();

	delete [] avail_;
	delete [] work_;

	for (int i = capacity_; i;)
		delete queue_[--i];

	delete [] queue_;
}

void Queue::Push(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key)
{
	mutex_.Lock();
	while (avail_[front_] < 0)
		empty_.Wait(mutex_);
	int seq = avail_[front_];
	Task *task = queue_[seq];
	task->Assign(fun, db, key);
	avail_[front_] = -1;
	work_[front_++] = seq;
	if (front_ == capacity_)
		front_ = 0;
	mutex_.Unlock();
	ready_.Signal();
}

void Queue::Pull()
{
	mutex_.Lock();
	while (work_[work_back_] < 0 && !clear_)
		ready_.Wait(mutex_);

	if (clear_) {
		mutex_.Unlock();
		return ;
	}

	int seq = work_[work_back_];
	Task *task = queue_[seq];
	work_[work_back_++] = -1;
	if (work_back_ == capacity_)
		work_back_ = 0;
	mutex_.Unlock();

	(*task)();

	mutex_.Lock();
	avail_[avail_back_++] = seq;
	if (avail_back_ == capacity_)
		avail_back_ = 0;
	mutex_.Unlock();
	empty_.Signal();
}

void Queue::Clear()
{
	mutex_.Lock();
	while (front_ != avail_back_ || front_ != work_back_)
		empty_.Wait(mutex_);

	clear_ = true;
	mutex_.Unlock();

	ready_.Broadcast();
}

} // namespace Mushroom
