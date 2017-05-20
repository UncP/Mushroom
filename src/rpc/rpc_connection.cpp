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

RpcConnection::RpcConnection(const EndPoint &server, Poller *poller, float error_rate)
:Connection(server, poller), error_rate_(error_rate), marshaller_(&input_, &output_)
{
	readcb_ = [this]() {
		for (; marshaller_.HasCompleteArgs();) {
			uint32_t rid;
			marshaller_ >> rid;
			spin_.Lock();
			auto it = futures_.find(rid);
			FatalIf(it == futures_.end(), "rpc id %u not called :(", rid);
			Future *fu = it->second;
			spin_.Unlock();
			fu->Notify();
		}
		if (input_.size())
			input_.Adjust();
	};
}

RpcConnection::RpcConnection(const Socket &socket, Poller *poller)
:Connection(socket, poller), marshaller_(&input_, &output_) { }

RpcConnection::~RpcConnection()
{
	for (auto e : futures_)
		delete e.second;
}

void RpcConnection::Disable()
{
	disable_ = 1;
}

void RpcConnection::Enable()
{
	disable_ = 0;
}

Marshaller& RpcConnection::GetMarshaller()
{
	return marshaller_;
}

} // namespace Mushroom
