/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:45
**/

#include "server.hpp"

namespace Mushroom {

Server::Server() { }

Server::~Server() { }

bool Server::Start()
{
	if (!socket_.Create()) {
		// Log(Error, "server socket create failed :(\n");
		return false;
	}
	if (!socket_.Bind()) {
		// Log(Error, "server socket bind port %hu failed :(\n", Port);
		return false;
	}
	if (!socket_.Listen()) {
		// Log(Error, "server socket listen failed :(\n");
		return false;
	}
	return true;
}

bool Server::Close()
{
	bool flag = socket_.Close();
	if (flag) {
		// Log(Info, "server closed")
	}
	return flag;
}

} // namespaceMushroom
