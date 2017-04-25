/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:01
**/

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "socket.hpp"

namespace Mushroom {

class Poller;

class Connection
{
	public:
		static enum { Invalid, Connected, Closed, Failed } State;

		Connection(const EndPoint &server);

	private:
		State   state_;
		Socket  socket_;
		Poller *poller_;
};

} // namespace Mushroom

#endif /* _CONNECTION_HPP_ */