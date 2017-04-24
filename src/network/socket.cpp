/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:23:53
**/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "socket.hpp"

namespace Mushroom {

Socket::Socket():fd_(-1) { }

Socket::~Socket() { }

int Socket::fd() const
{
	return fd_;
}

bool Socket::Create()
{
	fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return fd_ != -1;
}

bool Socket::Close()
{
	bool flag = true;
	if (fd_ != -1) {
		flag = !close(fd_);
		fd_ = -1;
	}
	return flag;
}

bool Socket::Connect(const EndPoint &end_point)
{
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family      = AF_INET;
	server.sin_port        = htons(ServerPort);
	server.sin_addr.s_addr = end_point.Address();
	return !connect(fd_, (const struct sockaddr *)&server, sizeof(server));
}

bool Socket::Bind()
{
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family      = AF_INET;
	server.sin_port        = htons(ServerPort);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	return !bind(fd_, (const struct sockaddr *)&server, sizeof(server));
}

bool Socket::Listen()
{
	return !listen(fd_, 1024);
}

bool Socket::Accept()
{
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	bool flag = !accept(fd_, (struct sockaddr *)&client, &len);
	// char buf[EndPoint::MaxLen];
	// assert(inet_ntop(AF_INET, &client.sin_addr, buf, EndPoint::MaxLen));
	// in_port_t port = ntohs(client.sin_port);
	// Log(Info, "connection from %15s  port: %d\n", buf, port);
	return flag;
}

bool Socket::SetOption(int value, bool flag)
{
	return !setsockopt(fd_, SOL_SOCKET, value, &flag, sizeof(flag));
}

bool Socket::GetOption(int value, bool *flag)
{
	return !getsockopt(fd_, SOL_SOCKET, value, flag, sizeof(*flag));
}

bool Socket::SetNonBlock(bool flag)
{
	int value = fcntl(fd_, F_GETFL, 0);
	if (value < 0) return false;
	if (flag)
	value = flag ? (value | O_NONBLOCK) : (value & ~O_NONBLOCK);
	return !fcntl(fd_, F_SETFL, value);
}

bool Socket::AddFlag(int flag)
{
	int value = fcntl(fd_, F_GETFL, 0);
	return !fcntl(fd_, F_SETFL, value | flag);
}

} // namespace Mushroom
