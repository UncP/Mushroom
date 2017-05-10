/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 11:23:08
**/

#include "rpc_server.hpp"
#include "../network/eventbase.hpp"
#include "rpc_connection.hpp"
#include "../network/channel.hpp"

namespace Mushroom {

RpcServer::RpcServer(EventBase *event_base):Server(event_base) { }

RpcServer::~RpcServer() { }

void RpcServer::HandleAccept()
{
	int fd = socket_.Accept();
	if (fd < 0) {
		Error("socket accept failed, %s :(", strerror(errno));
		return ;
	}
	RpcConnection *con = new RpcConnection(Socket(fd), event_base_->GetPoller());
	connections_.push_back((Connection *)con);
	con->OnRead([con, this]() {
		Marshaller &mar = con->GetMarshaller();
		if (mar.HasCompleteArgs()) {
			uint32_t id;
			mar >> id;
			auto it = services_.find(id);
			FatalIf(it == services_.end(), "rpc call %u not registered :(", id);
			RPC &rpc = it->second;
			rpc.GetReady(mar);
			rpc();
			Channel *ch = con->GetChannel();
			if (!ch->CanWrite())
				ch->EnableWrite(true);
		}
	});
}

} // namespace Mushroom
