/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:50:45
**/

#include "../log/log.hpp"
#include "server.hpp"
#include "channel.hpp"
#include "connection.hpp"
#include "poller.hpp"

namespace Mushroom {

Server::Server():listen_(0), poller_(new Poller()), running_(false), connectcb_(0) { }

Server::~Server()
{
	delete listen_;

	if (socket_.Valid())
		socket_.Close();

	for (auto e : connections_)
		delete e;

	delete poller_;
}

bool Server::Start()
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	FatalIf(!socket_.SetResuseAddress(), "socket option set failed :(", strerror(errno));
	FatalIf(!socket_.Bind(), "socket bind failed, %s :(", strerror(errno));
	FatalIf(!socket_.Listen(), "socket listen failed, %s :(", strerror(errno));
	listen_ = new Channel(socket_.fd(), ReadEvent, poller_);
	listen_->OnRead([this]() { Accept(); });
	running_ = true;
	return true;
}

bool Server::Close()
{
	Info("server closed ;)");
	running_ = false;
	return socket_.Close();
}

void Server::Run()
{
	for (; running_;) {
		poller_->LoopOnce();
	}
}

void Server::OnConnect(const ConnectCallBack &connectcb)
{
	connectcb_ = connectcb;
}

void Server::Accept()
{
	int fd = socket_.Accept();
	if (fd < 0) {
		Error("socket accept failed, %s :(", strerror(errno));
		return ;
	}
	Connection *con = new Connection(Socket(fd), ReadEvent | WriteEvent, poller_);
	if (connectcb_)
		connectcb_(con);
	connections_.push_back(con);
}

} // namespaceMushroom
