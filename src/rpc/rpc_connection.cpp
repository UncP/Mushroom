/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:15
**/

#include <cassert>

#include "rpc_connection.hpp"

namespace Mushroom {

atomic_32_t RpcConnection::RpcId(0);

RpcConnection::RpcConnection(const EndPoint &server, Poller *poller)
:Connection(server, poller), marshaller_(&input_, &output_)
{
	readcb_ = [this]() {
		if (marshaller_.HasCompleteArgs()) {
			uint32_t rid;
			marshaller_ >> rid;
			auto it = futures_.find(rid);
			assert(it != futures_.end());
			it->second->Notify();
		}
	};
}

RpcConnection::RpcConnection(const Socket &socket, Poller *poller)
:Connection(socket, poller), marshaller_(&input_, &output_) { }

RpcConnection::~RpcConnection()
{
	for (auto e : futures_)
		delete e.second;
}

Marshaller& RpcConnection::GetMarshaller()
{
	return marshaller_;
}

Channel* RpcConnection::GetChannel()
{
	return channel_;
}

} // namespace Mushroom
