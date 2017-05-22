/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-06 23:47:45
**/

#include <unistd.h>

#include "../include/bounded_queue.hpp"
#include "../include/thread_pool.hpp"
#include "time.hpp"
#include "eventbase.hpp"
#include "../log/log.hpp"
#include "poller.hpp"
#include "channel.hpp"
#include "socket.hpp"

namespace Mushroom {

static const int MaxTimeout = 0x7FFFFFFF;

EventBase::EventBase(int thread_num, int queue_size)
:running_(true), poller_(new Poller()), next_time_out_(MaxTimeout), seq_(0),
queue_(new BoundedQueue<Task>(queue_size)), pool_(new ThreadPool<Task>(queue_, thread_num))
{
	int r = pipe(wake_up_);
	FatalIf(r, "pipe failed, %s :(", strerror(errno));
	FatalIf(!Socket(wake_up_[0]).SetNonBlock(), "wake up fd set non-block failed :(");
	channel_ = new Channel(wake_up_[0], poller_, 0, 0);
	channel_->OnRead([this]() {
		char buf;
		ssize_t r = read(channel_->fd(), &buf, sizeof(buf));
		if (r == sizeof(buf)) {
			// Info("wake up");
		} else {
			Fatal("pipe read error, %s :(", strerror(errno));
		}
	});
	pid_ = pthread_self();
}

EventBase::~EventBase()
{
	close(wake_up_[1]);
	delete channel_;
	delete poller_;
	delete pool_;
	delete queue_;
}

Poller* EventBase::GetPoller()
{
	return poller_;
}

void EventBase::Loop()
{
	while (running_) {
		poller_->LoopOnce(std::min(5000, next_time_out_));
		HandleTimeout();
	}
	poller_->LoopOnce(0);
}

void EventBase::Exit()
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return ;
	}
	running_ = false;
	WakeUp();
	repeat_.clear();
	pending_.clear();
	mutex_.Unlock();
}

void EventBase::WakeUp()
{
	ssize_t r = write(wake_up_[1], "", 1);
	FatalIf(r <= 0, "wake up failed, %s :(", strerror(errno));
}

void EventBase::Refresh()
{
	if (pending_.empty()) {
		next_time_out_ = MaxTimeout;
	} else {
		auto &it = pending_.begin()->first;
		int64_t tmp = it.first - Time::Now();
		next_time_out_ = (tmp <= 0) ? 0 : int(tmp);
	}
}

void EventBase::RunNow(const Task &task)
{
	RunAfter(0, task);
}

TimerId EventBase::RunAfter(int64_t milli_sec, const Task &task)
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return TimerId();
	}
	TimerId id { Time::Now() + milli_sec, seq_++};
	pending_.insert({id, task});
	if (pthread_self() != pid_) {
		WakeUp();
	} else {
		Refresh();
	}
	mutex_.Unlock();
	return id;
}

TimerId EventBase::RunEvery(int64_t milli_sec, const Task &task)
{
	mutex_.Lock();
	if (!running_) {
		mutex_.Unlock();
		return TimerId();
	}
	uint32_t seq = seq_++;
	TimeRep rep {milli_sec, Time::Now()};
	TimerId id {rep.second, seq};
	repeat_.insert({seq, rep});
	pending_.insert({id, task});
	if (pthread_self() != pid_) {
		WakeUp();
	} else {
		Refresh();
	}
	mutex_.Unlock();
	return {milli_sec, seq};
}

void EventBase::Cancel(const TimerId &id)
{
	mutex_.Lock();
	auto rit = repeat_.find(id.second);
	if (rit != repeat_.end()) {
		repeat_.erase(rit);
		auto it = pending_.find({rit->second.second, id.second});
		if (it != pending_.end())
			pending_.erase(it);
	} else {
		auto it = pending_.find(id);
		if (it != pending_.end())
			pending_.erase(it);
	}
	mutex_.Unlock();
}

void EventBase::HandleTimeout()
{
	TimerId now { Time::Now(), 0xFFFFFFFF};
	mutex_.Lock();
	for (; !pending_.empty() && pending_.begin()->first <= now; ) {
		queue_->Push(pending_.begin()->second);
		TimerId id = pending_.begin()->first;
		auto it = repeat_.find(id.second);
		if (it != repeat_.end()) {
			TimerId nid { now.first + it->second.first, id.second };
			it->second.second = nid.first;
			pending_.insert({nid, pending_.begin()->second});
		}
		pending_.erase(pending_.begin());
	}
	Refresh();
	mutex_.Unlock();
}

} // namespace Mushroom
