/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 11:23:08
**/

#include "rpc_server.hpp"
#include "../network/connection.hpp"
#include "../network/poller.hpp"

namespace Mushroom {

RpcServer::RpcServer() { }

RpcServer::~RpcServer() { }

void RpcServer::HandleAccept()
{
	int fd = socket_.Accept();
	if (fd < 0) {
		Error("socket accept failed, %s :(", strerror(errno));
		return ;
	}
	Connection *con = new Connection(Socket(fd), ReadEvent | WriteEvent, poller_);
	connections_.push_back(con);
	con->OnRead([=]() {
		Marshaller marshaller(con->GetInput(), con->GetOutput());
		for (; !marshaller.HasCompleteArgs();) {
			uint32_t id;
			marshaller >> id;
			FatalIf(services_.find(id) == services_.end(), "rpc call %u not registered :(", id);
			services_[id](marshaller);
		}
		con->HandleWrite();
	});
}

} // namespace Mushroom
