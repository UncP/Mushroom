/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:13:14
**/

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "socket.hpp"
#include "endpoint.hpp"

namespace Mushroom {

class Client
{
	public:
		Client();

		bool Connect(const EndPoint &server);

		void OnRead();

		void OnWrite();

		bool Close();

	private:
		Socket   socket_;
		EndPoint server_;
};

} // namespace Mushroom

#endif /* _CLIENT_HPP_ */