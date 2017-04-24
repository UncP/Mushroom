/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:22:54
**/

#include "client.hpp"

namespace Mushroom {

Client::Client() { }

bool Client::Connect(const EndPoint &server)
{
	if (!socket_.Create()) {
		// Log(Error, "client socket create failed :(\n");
		return false;
	}
	if (!socket_.Connect(server)) {
		// Log(Error, "connect server failed :(\n");
		return false;
	}
	server_ = server;
	return true;
}

bool Close()
{
	return socket_.Close();
}


} // namespace Mushroom
