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

class Connection
{
	public:
		enum State { Invalid, HandShake, Connected, Closed, Failed };

		Connection(const Socket &socket);

		Connection(const EndPoint &server);

		bool Success() const;

		int fd() const;

		uint32_t Events() const;

		bool Close();

	private:
		State    state_;
		Socket   socket_;
		uint32_t events_;
		EndPoint local_;
		EndPoint peer_;
};

} // namespace Mushroom

#endif /* _CONNECTION_HPP_ */