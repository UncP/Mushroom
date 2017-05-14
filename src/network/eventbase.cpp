/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:47:45
**/

#include <unistd.h>

#include "time.hpp"
#include "eventbase.hpp"
#include "../log/log.hpp"
#include "poller.hpp"
#include "channel.hpp"
#include "socket.hpp"

namespace Mushroom {

static const int32_t MaxTimeout = 0x7FFFFFFF;

EventBase::EventBase(int thread_num, int queue_size)
:running_(true), poller_(new Poller()), next_time_out_(MaxTimeout), seq_(0),
queue_(new BoundedQueue<Task>(queue_size)), pool_(new ThreadPool<Task>(thread_num, queue_))
{
	int r = pipe(wake_up_);
	FatalIf(r, "pipe failed, %s :(", strerror(errno));
	FatalIf(!Socket(wake_up_[0]).SetNonBlock(), "wake up fd set non-block failed :(");
	Channel *ch = new Channel(wake_up_[0], poller_, 0, 0);
	ch->OnRead([ch]() {
		char buf[1];
		ssize_t r = read(ch->fd(), buf, sizeof(buf));
		if (r >= 0) {
			Info("wake up");
			delete ch;
		} else if (errno == EINTR) {
		} else {
			Fatal("pipe read error, %s :(", strerror(errno));
		}
	});
}

EventBase::~EventBase()
{
	delete poller_;
	close(wake_up_[1]);
	delete pool_;
	delete queue_;
}

Poller* EventBase::GetPoller()
{
	return poller_;
}

void EventBase::Loop()
{
	while (running_)
		poller_->LoopOnce(std::min(5000, next_time_out_.get()));
	poller_->LoopOnce(0);
}

void EventBase::Exit()
{
	if (running_) {
		running_ = false;
		WakeUp();
	}
}

void EventBase::WakeUp()
{
	ssize_t r = write(wake_up_[1], "", 1);
	FatalIf(r <= 0, "wake up failed, %s :(", strerror(errno));
}

void EventBase::RunNow(const Task &task)
{
	queue_->Push(task);
}

TimerId EventBase::RunAfter(int64_t milli_sec, const Task &task)
{
	TimerId id   = seq_++;
	int64_t then = Time::Now() + milli_sec;
	mutex_.Lock();
	pending_.insert({{then, id}, task});
	Refresh(false);
	mutex_.Unlock();
}

void EventBase::Refresh(bool lock)
{
	bool empty;
	if (lock) {
		mutex_.Lock();
		empty = pending_.empty();
		mutex_.Unlock();
	} else {
		empty = pending_.empty();
	}

	if (empty) {
		next_time_out_ = MaxTimeout;
	} else {
		auto &timer_id = pending_.begin()->first;
		int64_t tmp = timer_id.first - Time::Now();
		next_time_out_ = tmp < 0 ? 0 : int32_t(tmp);
	}
}

TimerId EventBase::RunEvery(int64_t milli_sec, const Task &task)
{
	queue_->Push(task);
}

void EventBase::Cancel(const TimerId &timer_id)
{
	mutex_.Lock();
	auto it = pending_.find(timer_id);
	if (it != pending_.end())
		pending_.erase(it);
	mutex_.Unlock();
}

void EventBase::HandleTimeout()
{
	TimerId now { Time::Now(), 0xFFFFFFFF};
	mutex_.Lock();
	for (; !pending_.empty() && pending_.begin()->first < now; ) {
		queue_->Push(pending_.begin()->second);
		pending_.erase(pending_.begin());
	}
	Refresh(false);
	mutex_.Unlock();
}

} // namespace Mushroom
