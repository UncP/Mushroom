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

class Poller
{
	public:
		Poller(const Socket &listen);

		~Poller();

		void AddChannel(Socket *socket);

		void UpdateChannel(Socket *socket);

		void RemoveChannel(Socket *socket);

		void LoopOnce();

	private:
		int                fd_;
		struct epoll_event events_[MaxEvents];
};

} // namespace Mushroom

#endif /* _POLLER_HPP_ */