/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 10:16:38
**/

#include "rpc_client.hpp"

namespace Mushroom {

RpcClient::RpcClient():connection_(0), marshal_(0), connectcb_(0) { }

RpcClient::~RpcClient()
{
	delete marshal_;
	delete connection_;
}

void RpcClient::OnConnect(const ConnectCallBack &connectcb)
{
	connectcb_ = connectcb;
}

bool RpcClient::Connect(const EndPoint &server)
{
	connection_ = new Connection(server);
	if (!connection_->Success()) {
		delete connection_;
		connection_ = 0;
		return false;
	}
	marshal_ = new Marshal(connection_->GetInput(), connection_->GetOutput());
	return true;
}

bool RpcClient::Close()
{
	if (connection_)
		return connection_->Close();
	return true;
}

} // namespace Mushroom
