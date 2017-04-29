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

Poller::Poller(Connection *connection):listen_(connection->socket())
{
	fd_ = epoll_create1(EPOLL_CLOEXEC);
	assert(fd_ >= 0);
	AddConnection(connection);
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
	assert(!epoll_ctl(fd_, EPOLL_CTL_ADD, connection->socket().fd(), &event));
}

void Poller::LoopOnce()
{
	int ready = epoll_wait(fd_, events_, MaxEvents, 1000);
	printf("ready %d\n", ready);
	for (; --ready >= 0; ) {
		Connection *connection = (Connection *)events_[ready].data.ptr;
		Socket socket = connection->socket();
		uint32_t event = events_[ready].events;
		if (socket == listen_) {
			int fd;
			if ((fd = socket.Accept()) >= 0) {
				Socket new_sock(fd);
				assert(new_sock.SetNonBlock());
				AddConnection(new Connection(new_sock, ReadEvent | WriteEvent));
				printf("new connection ;)\n");
			}
		} else if (event & ReadEvent) {
			connection->HandleRead();
		} else if (event & WriteEvent) {
			connection->HandleWrite();
		} else {
			assert(0);
		}
	}
}

} // namespace Mushroom
