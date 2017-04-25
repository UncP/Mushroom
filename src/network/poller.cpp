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

void Poller::AddChannel(Socket *socket)
{

}

void Poller::UpdateChannel(Socket *socket)
{
	// struct epoll_event event;
	// memset(&event, 0, sizeof(event));
}

void Poller::RemoveChannel(Socket *socket)
{

}

void Poller::LoopOnce()
{

}


} // namespace Mushroom
