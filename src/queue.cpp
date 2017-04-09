/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2016-10-17 14:21:54
**/

#include <cassert>

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

	assert(!pthread_mutex_init(mutex_, 0));
	assert(!pthread_cond_init(ready_, 0));
	assert(!pthread_cond_init(empty_, 0));
}

void Queue::Push(bool (MushroomDB::*(fun))(KeySlice *), MushroomDB *db, KeySlice *key)
{
	pthread_mutex_lock(mutex_);
	while (avail_[front_] < 0)
		pthread_cond_wait(empty_, mutex_);
	int seq = avail_[front_];
	Task *task = queue_[seq];
	task->Assign(fun, db, key);
	avail_[front_] = -1;
	work_[front_++] = seq;
	if (front_ == capacity_)
		front_ = 0;
	pthread_mutex_unlock(mutex_);
	pthread_cond_signal(ready_);
}

void Queue::Pull()
{
	pthread_mutex_lock(mutex_);
	while (work_[work_back_] < 0 && !clear_)
		pthread_cond_wait(ready_, mutex_);

	if (clear_) {
		pthread_mutex_unlock(mutex_);
		return ;
	}

	int seq = work_[work_back_];
	Task *task = queue_[seq];
	work_[work_back_++] = -1;
	if (work_back_ == capacity_)
		work_back_ = 0;
	pthread_mutex_unlock(mutex_);

	(*task)();

	pthread_mutex_lock(mutex_);
	avail_[avail_back_++] = seq;
	if (avail_back_ == capacity_)
		avail_back_ = 0;
	pthread_mutex_unlock(mutex_);
	if (avail_back_ == work_back_)
		pthread_cond_signal(empty_);
}

void Queue::Clear()
{
	pthread_mutex_lock(mutex_);
	while (front_ != avail_back_ || front_ != work_back_)
		pthread_cond_wait(empty_, mutex_);
	clear_ = true;
	pthread_mutex_unlock(mutex_);

	pthread_cond_broadcast(ready_);
}

Queue::~Queue()
{
	if (!clear_)
		Clear();

	assert(!pthread_mutex_destroy(mutex_));
	assert(!pthread_cond_destroy(ready_));
	assert(!pthread_cond_destroy(empty_));

	delete [] avail_;
	delete [] work_;

	for (int i = capacity_; i;)
		delete queue_[--i];
	delete [] queue_;
}

} // namespace Mushroom
