/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:39
**/

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "endpoint.hpp"
#include "socket.hpp"

namespace Mushroom {

class Poller;

class Server
{
	public:
		Server();

		~Server();

		bool Start();

		bool Close();

	private:
		EndPoint end_point_;
		Socket   socket_;
		Poller  *poller_;
};

} // namespace Mushroom

#endif /* _SERVER_HPP_ */