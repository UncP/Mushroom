/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include "connection.hpp"

#include <sys/poll.h>
#include <cassert>

namespace Mushroom {

Connection::Connection(const EndPoint &server):state_(Invalid)
{
	if (!socket_.Create()) {
		// Log(Error, "socket create failed :(\n");
		return ;
	}
	if (!socket_.Connect(server)) {
		state_ = Failed;
		// Log(Error, "connect server %s failed :(\n", server.ToString());
		return ;
	}
	struct pollfd pfd;
	pfd.fd = socket_.fd();
	pfd.events = POLLOUT | POLLERR;
	int r = poll(&pfd, 1, 0);
	if (r == 1 && pfd.revents == POLLOUT) {
		state_ = Connected;
		assert(socket_.GetSockName(&local_));
		assert(socket_.GetPeerName(&peer_));
	} else {
		state_ = Failed;
	}
}

Connection::Connection(const Socket &socket):state_(Invalid), socket_(socket)
{
	if (socket_.GetSockName(&local_) && socket_.GetPeerName(&peer_))
		state_ = Connected;
}

bool Connection::Success() const
{
	return state_ == Connected;
}

bool Connection::Close()
{
	if (state_ == Connected && !socket_.Close())
		return false;
	state_ = Invalid;
	return true;
}

} // namespace Mushroom
