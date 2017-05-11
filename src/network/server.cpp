/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:45
**/

#include "../log/log.hpp"
#include "eventbase.hpp"
#include "server.hpp"
#include "channel.hpp"
#include "connection.hpp"

namespace Mushroom {

Server::Server(EventBase *event_base)
:event_base_(event_base), listen_(0), connectcb_(0) { }

Server::~Server()
{
	delete listen_;

	socket_.Close();

	for (auto e : connections_)
		delete e;
}

void Server::Start()
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	FatalIf(!socket_.SetResuseAddress(), "socket option set failed :(", strerror(errno));
	FatalIf(!socket_.Bind(), "socket bind failed, %s :(", strerror(errno));
	FatalIf(!socket_.Listen(), "socket listen failed, %s :(", strerror(errno));
	listen_ = new Channel(socket_.fd(), event_base_->GetPoller(), [this]() { HandleAccept(); }, 0);
}

void Server::OnConnect(const ConnectCallBack &connectcb)
{
	connectcb_ = connectcb;
}

void Server::HandleAccept()
{
	int fd = socket_.Accept();
	if (fd < 0) {
		Error("socket accept failed, %s :(", strerror(errno));
		return ;
	}
	Connection *con = new Connection(Socket(fd), event_base_->GetPoller());
	if (connectcb_)
		connectcb_(con);
	connections_.push_back(con);
}

} // namespaceMushroom
