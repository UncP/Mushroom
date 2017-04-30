/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include "../log/log.hpp"
#include "connection.hpp"
#include "channel.hpp"

namespace Mushroom {

Connection::Connection(const EndPoint &server)
:connected_(false), channel_(0), readcb_(0), writecb_(0), sendcb_(0)
{
	FatalIf(!socket_.Create(), "socket create failed :(", strerror(errno));
	FatalIf(!socket_.Connect(server), "socket connect server %s failed, %s :(",
		server.ToString().c_str(), strerror(errno));
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed :(", strerror(errno));
	connected_ = true;
}

Connection::Connection(const Socket &socket, uint32_t events, Poller *poller)
:socket_(socket), readcb_(0), writecb_(0), sendcb_(0)
{
	channel_ = new Channel(socket.fd(), events, poller);
	channel_->OnRead([this]() { this->HandleRead(); });
	channel_->OnWrite([this]() { this->HandleWrite(); });
}

Connection::~Connection()
{
	delete channel_;

	if (socket_.Valid())
		socket_.Close();
}

bool Connection::Close()
{
	Info("connection closed ;)");
	return socket_.Close();
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
	input_.Clear();
	uint32_t read = socket_.Read(input_.end(), input_.space());

	if (!read) {
		channel_->readcb_ = 0;
		channel_->writecb_ = 0;
		Channel *tmp = channel_;
		channel_ = 0;
		delete tmp;
		return ;
	}
	input_.Expand(read);
	if (readcb_)
		readcb_();
}

void Connection::HandleWrite()
{
	output_.Consume(socket_.Write(output_.begin(), output_.size()));
	if (writecb_)
		writecb_();
}

void Connection::Send(const char *str)
{
	Send(str, strlen(str));
}

void Connection::Send(Buffer &buffer)
{
	buffer.Consume(Send(buffer.begin(), buffer.size()));
}

uint32_t Connection::Send(const char *str, uint32_t len)
{
	uint32_t sent = socket_.Write(str, len);
	output_.Append(str, sent);
	if (sendcb_)
		sendcb_();
	return sent;
}

} // namespace Mushroom
