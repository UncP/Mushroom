/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-24 11:21:25
**/

#ifndef _POLLER_HPP_
#define _POLLER_HPP_

#include <sys/epoll.h>

#include "socket.hpp"

const uint32_t ReadEvent  = EPOLLIN;
const uint32_t WriteEvent = EPOLLOUT;
const uint32_t MaxEvents  = 1024;

namespace Mushroom {

class Connection;

class Poller
{
	public:
		Poller(Connection *connection);

		~Poller();

		void LoopOnce();

	private:
		Socket             listen_;
		int                fd_;
		struct epoll_event events_[MaxEvents];

		void AddConnection(Connection *connection);
};

} // namespace Mushroom

#endif /* _POLLER_HPP_ */