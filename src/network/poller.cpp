/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-24 11:31:12
**/

#include <unistd.h>
#include <cstring>
#include <cassert>

#include "poller.hpp"
#include "channel.hpp"

namespace Mushroom {

Poller::Poller()
{
	assert((fd_ = epoll_create1(EPOLL_CLOEXEC)) >= 0);
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
	assert(!epoll_ctl(fd_, EPOLL_CTL_ADD, channel->fd(), &event));
}

void Poller::UpdateChannel(Channel *channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	assert(!epoll_ctl(fd_, EPOLL_CTL_MOD, channel->fd(), &event));
}

void Poller::RemoveChannel(Channel *channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	assert(!epoll_ctl(fd_, EPOLL_CTL_DEL, channel->fd(), &event));
}

void Poller::LoopOnce(int ms)
{
	int ready = epoll_wait(fd_, events_, MaxEvents, ms);
	assert(!(ready == -1 && errno != EINTR));
	for (; --ready >= 0; ) {
		Channel *channel = (Channel *)events_[ready].data.ptr;
		uint32_t event = events_[ready].events;
		if (event & ReadEvent) {
			channel->HandleRead();
		} else if (event & WriteEvent) {
			channel->HandleWrite();
		} else {
			assert(0);
		}
	}
}

} // namespace Mushroom
