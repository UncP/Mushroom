/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:22:54
**/

#include "client.hpp"

namespace Mushroom {

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
	end_point_ = server;
	return true;
}

} // namespace Mushroom
