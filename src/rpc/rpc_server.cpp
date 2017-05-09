/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 11:23:08
**/

#include "rpc_server.hpp"
#include "../include/bounded_queue.hpp"
#include "../include/thread_pool.hpp"
#include "../network/eventbase.hpp"
#include "rpc_connection.hpp"

namespace Mushroom {

RpcServer::RpcServer(EventBase *event_base):Server(event_base)
{
	queue_       = new BoundedQueue<RPC>(64, []() { return new RPC(); });
	thread_pool_ = new ThreadPool<RPC>(queue_, 1);
}

RpcServer::~RpcServer()
{
	delete queue_;
	delete thread_pool_;
}

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
		Marshaller &marshaller = con->Marshal();
		if (marshaller.HasCompleteArgs()) {
			uint32_t id;
			marshaller >> id;
			auto it = services_.find(id);
			FatalIf(it == services_.end(), "rpc call %u not registered :(", id);
			RPC *rpc = queue_->Get();
			*rpc = RPC(marshaller, it->second);
			queue_->Push();
		}
	});
}

} // namespace Mushroom
