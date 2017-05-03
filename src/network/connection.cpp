/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include <unistd.h>

#include "../log/log.hpp"
#include "connection.hpp"
#include "channel.hpp"

namespace Mushroom {

Connection::Connection(const EndPoint &server)
:connected_(false), channel_(0), readcb_(0), writecb_(0), sendcb_(0)
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	if (!socket_.Connect(server)) {
		Error("socket connect server %s failed, %s :(", server.ToString().c_str(), strerror(errno));
		return ;
	}
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed :(", strerror(errno));
	connected_ = true;
}

Connection::Connection(const Socket &socket, uint32_t events, Poller *poller)
:socket_(socket), readcb_(0), writecb_(0), sendcb_(0)
{
	channel_ = new Channel(socket.fd(), events, poller);
	channel_->OnRead([this]() { this->HandleRead(); });
	channel_->OnWrite([this]() { this->HandleWrite(); });
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed :(", strerror(errno));
	connected_ = true;
}

Connection::~Connection()
{
	if (socket_.Valid())
		socket_.Close();
}

bool Connection::Close()
{
	if (socket_.Valid()) {
		Info("close connection ;)");
		connected_ = false;
		delete channel_;
		return socket_.Close();
	}
	return true;
}

bool Connection::Success() const
{
	return connected_;
}

Buffer& Connection::GetInput()
{
	return input_;
}

Buffer& Connection::GetOutput()
{
	return output_;
}

void Connection::OnRead(const ReadCallBack &readcb)
{
	readcb_ = readcb;
}

void Connection::OnWrite(const WriteCallBack &writecb)
{
	writecb_ = writecb;
}

void Connection::OnSend(const SendCallBack &sendcb)
{
	sendcb_ = sendcb;
}

void Connection::HandleRead()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	input_.Clear();
	bool blocked = false;
	uint32_t read = socket_.Read(input_.end(), input_.space(), &blocked);
	if (!read && !blocked) {
		Error("server closed :(");
		Close();
		return ;
	}
	input_.AdvanceTail(read);
	if (readcb_ && read)
		readcb_();
}

void Connection::HandleWrite()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	output_.AdvanceHead(socket_.Write(output_.begin(), output_.size()));
	if (writecb_ && output_.empty())
		writecb_();
}

void Connection::Send(const char *str)
{
	Send(str, strlen(str));
}

void Connection::Send(Buffer &buffer)
{
	output_.Expand(buffer.begin(), buffer.size());
	SendOutput();
}

void Connection::Send(const char *str, uint32_t len)
{
	output_.Expand(str, len);
	SendOutput();
}

void Connection::SendOutput()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	output_.AdvanceHead(socket_.Write(output_.begin(), output_.size()));
	if (sendcb_ && output_.empty())
		sendcb_();
}

} // namespace Mushroom
