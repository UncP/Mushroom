/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-24 11:31:12
**/

#include <unistd.h>
#include <cstring>

#include "../log/log.hpp"
#include "poller.hpp"
#include "channel.hpp"

namespace Mushroom {

Poller::Poller()
{
	FatalIf((fd_ = epoll_create1(EPOLL_CLOEXEC)) < 0,
		"epoll create failed, %s :(", strerror(errno));
}

Poller::~Poller()
{
	close(fd_);
}

void Poller::AddChannel(Channel *channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	FatalIf(epoll_ctl(fd_, EPOLL_CTL_ADD, channel->fd(), &event),
		"epoll add event failed, %s :(", strerror(errno));
}

void Poller::UpdateChannel(Channel *channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	FatalIf(epoll_ctl(fd_, EPOLL_CTL_MOD, channel->fd(), &event),
		"epoll update event failed, %s :(", strerror(errno));
}

void Poller::RemoveChannel(Channel *channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	FatalIf(epoll_ctl(fd_, EPOLL_CTL_DEL, channel->fd(), &event),
		"epoll remove event failed, %s fd: %d :(", channel->fd(), strerror(errno));
}

void Poller::LoopOnce(int ms)
{
	int ready = epoll_wait(fd_, events_, MaxEvents, ms);
	FatalIf(ready == -1 && errno != EINTR, "epoll wait failed, %s :(", strerror(errno));
	for (; --ready >= 0; ) {
		Channel *channel = (Channel *)events_[ready].data.ptr;
		uint32_t event = events_[ready].events;
		if (event & ReadEvent) {
			channel->HandleRead();
		} else if (event & WriteEvent) {
			channel->HandleWrite();
		} else {
			Fatal("unexpected epoll event :(");
		}
	}
}

} // namespace Mushroom
