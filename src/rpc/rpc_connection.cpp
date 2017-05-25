/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-02 21:22:15
**/

#include <cassert>

#include "../log/log.hpp"
#include "rpc_connection.hpp"

namespace Mushroom {

atomic_32_t RpcConnection::RpcId(0);

RpcConnection::RpcConnection(const EndPoint &server, Poller *poller, float error_rate)
:Connection(server, poller), disable_(0), error_rate_(error_rate), marshaller_(&input_, &output_)
{
	readcb_ = [this]() {
		uint32_t packet_size;
		if (disable_.get()) {
			input_.Clear();
			return ;
		}
		for (; (packet_size = marshaller_.HasCompleteArgs());) {
			uint32_t rid;
			marshaller_ >> rid;
			spin_.Lock();
			auto it = futures_.find(rid);
			if (it == futures_.end()) {
				Info("rpc id %u not called or expired :(", rid);
				marshaller_.Dump(packet_size);
			} else {
				Func func(std::move(it->second));
				futures_.erase(it);
				spin_.Unlock();
				func();
			}
		}
		if (input_.size())
			input_.Adjust();
	};
}

RpcConnection::RpcConnection(const Socket &socket, Poller *poller)
:Connection(socket, poller), marshaller_(&input_, &output_) { }

RpcConnection::~RpcConnection() { }

void RpcConnection::Disable()
{
	disable_ = 1;
}

bool RpcConnection::Disabled()
{
	return disable_.get();
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
