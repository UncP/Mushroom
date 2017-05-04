/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:15
**/

#include "../log/log.hpp"
#include "../network/channel.hpp"
#include "rpc_connection.hpp"

namespace Mushroom {

RpcConnection::RpcConnection(const EndPoint &server)
:Connection(server), marshaller_(input_, output_) { }

RpcConnection::RpcConnection(const Socket &socket, uint32_t events, Poller *poller)
:socket_(socket), readcb_(0), writecb_(0), sendcb_(0)
{
	channel_ = new Channel(socket.fd(), events, poller);
	channel_->OnRead([this]() { this->HandleRead(); });
	channel_->OnWrite([this]() { this->HandleWrite(); });
	FatalIf(!socket_.SetNonBlock(), "socket set non-block failed, %s :(", strerror(errno));
	connected_ = true;
}

void RpcConnection::HandleRead()
{
	if (!connected_) {
		Error("connection has closed :(");
		return ;
	}
	input_.Reset();
	bool blocked = false;
	uint32_t read = socket_.Read(input_.end(), input_.space(), &blocked);
	if (!read && !blocked) {
		Error("server closed :(");
		Close();
		return ;
	}
	input_.AdvanceTail(read);
	marshaller_
	if (readcb_ && read)
		readcb_();
}

void RpcConnection::HandleWrite()
{

}

RpcConnection::~RpcConnection() { }

} // namespace Mushroom
