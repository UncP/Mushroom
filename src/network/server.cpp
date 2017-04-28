/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:45
**/

#include "server.hpp"
#include "connection.hpp"
#include "poller.hpp"

#include <cassert>

namespace Mushroom {

Server::Server():connection_(0), poller_(0), running_(false) { }

Server::~Server()
{
	delete connection_;
	delete poller_;
}

bool Server::Start()
{
	Socket socket;
	if (!socket.Create())
		return false;
	if (!socket.Bind())
		return false;
	if (!socket.Listen())
		return false;
	connection_ = new Connection(socket, ReadEvent);
	poller_ = new Poller(connection_);
	running_ = true;
	return true;
}

void Server::Stop()
{
	running_ = false;
}

bool Server::Close()
{
	running_ = false;
	if (connection_)
		return connection_->Close();
	else
		return true;
}

void Server::Run()
{
	for (; running_;) {
		poller_->LoopOnce();
	}
}

} // namespaceMushroom
