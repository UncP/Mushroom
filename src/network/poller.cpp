/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-24 11:31:12
**/

#include <unistd.h>
#include <cassert>
#include <cstring>

#include "poller.hpp"
#include "connection.hpp"

namespace Mushroom {

Poller::Poller()
{
	fd_ = epoll_create1(EPOLL_CLOEXEC);
	assert(fd_ >= 0);
}

Poller::~Poller()
{
	close(fd_);
}

void Poller::AddConnection(Connection *connection)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = connection->Events();
	event.data.ptr = connection;
	assert(!epoll_ctl(fd_, EPOLL_CTL_ADD, connection->fd(), &event));
}

void Poller::UpdateConnection(Connection *connection)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = connection->Events();
	event.data.ptr = connection;
	assert(!epoll_ctl(fd_, EPOLL_CTL_MOD, connection->fd(), &event));
}

void Poller::RemoveConnection(Connection *connection)
{

}

void Poller::LoopOnce()
{
	int ready = epoll_wait(fd_, events_, MaxEvents, -1);
	assert(ready != -1 || errno != EINTR);
	for (; --ready >= 0; ) {
		Connection *connection = (Connection *)events_[ready].data.ptr;
		uint32_t event = events_[ready].events;
		if (event & ReadEvent)
			// connection->HandleRead();
			;
		else if (event & WriteEvent)
			// connection->HandleWrite();
			;
		else
			assert(0);
	}
}

} // namespace Mushroom
