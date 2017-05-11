/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:15
**/

#include <cassert>

#include "../log/log.hpp"
#include "rpc_connection.hpp"

namespace Mushroom {

atomic_32_t RpcConnection::RpcId(0);

RpcConnection::RpcConnection(const EndPoint &server, Poller *poller)
:Connection(server, poller), marshaller_(&input_, &output_)
{
	readcb_ = [this]() {
		for (; marshaller_.HasCompleteArgs();) {
			uint32_t rid;
			marshaller_ >> rid;
			auto it = futures_.find(rid);
			FatalIf(it == futures_.end(), "rpc id %u not called :(", rid);
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
	assert(futures_.size() == RpcId.get());
}

Marshaller& RpcConnection::GetMarshaller()
{
	return marshaller_;
}

} // namespace Mushroom
