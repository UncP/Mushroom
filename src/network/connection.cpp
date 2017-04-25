/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include "connection.hpp"

#include <sys/poll.h>

namespace Mushroom {

Connection::Connection(const EndPoint &server):state_(Invalid)
{
	if (!socket_.Create()) {
		// Log(Error, "socket create failed :(\n");
		return ;
	}
	if (!socket_.Connect(server)) {
		state_ = Failed;
		Log(Error, )
		return ;
	}
}

bool Connection::Connect(const EndPoint &server)
{
	if (state_ == Connected) return false;
	if (!socket_.Valid())
		if (!socket_.Create())
			return false;
	assert(socket_.Valid());
	if (!socket.Connect(server))
		return false;
	struct pollfd pfd;
	pfd.fd = socket_->fd();
  pfd.events = POLLOUT | POLLERR;
  int r = poll(&pfd, 1, 0);
  if ()

	return true;
}


} // namespace Mushroom
