/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:01
**/

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "socket.hpp"
#include "endpoint.hpp"

namespace Mushroom {

class Poller;

class Connection
{
	public:
		enum State { Invalid, Connected, Closed, Failed };

		Connection(const Socket &socket);

		Connection(const EndPoint &server);

		bool Success() const;

		bool Close();

	private:
		State    state_;
		Socket   socket_;
		EndPoint local_;
		EndPoint peer_;
		Poller  *poller_;
};

} // namespace Mushroom

#endif /* _CONNECTION_HPP_ */