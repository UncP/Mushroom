/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-25 22:11:08
**/

#include <cassert>
#include <cstring>

#include "connection.hpp"
#include "poller.hpp"

namespace Mushroom {

Connection::Connection(const EndPoint &server):events_(ReadEvent | WriteEvent), state_(false),
readcb_(0), writecb_(0)
{
	if (!socket_.Create())
		return ;
	if (!socket_.Connect(server))
		return ;
	assert(socket_.SetNonBlock());
	state_ = true;
}

Connection::Connection(const Socket &socket, uint32_t events)
:socket_(socket), events_(events) { }

bool Connection::Success() const
{
	return state_;
}

Socket Connection::socket() const
{
	return socket_;
}

uint32_t Connection::Events() const
{
	return events_;
}

Buffer& Connection::GetInput()
{
	return input_;
}

Buffer& Connection::GetOutput()
{
	return output_;
}

bool Connection::Close()
{
	return socket_.Close();
}

void Connection::OnRead(const ReadCallBack &readcb)
{
	readcb_ = readcb;
}

void Connection::OnWrite(const WriteCallBack &writecb)
{
	writecb_ = writecb;
}

void Connection::HandleRead()
{
	input_.Clear();
	input_.Expand(socket_.Read(input_.end(), input_.space()));
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
	return sent;
}

} // namespace Mushroom
