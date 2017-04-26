/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-23 10:22:54
**/

#include "client.hpp"
#include "connection.hpp"

namespace Mushroom {

Client::Client():connection_(0) { }

bool Client::Connect(const EndPoint &server)
{
	connection_ = new Connection(server);
	if (!connection_->Success()) {
		delete connection_;
		connection_ = 0;
		return false;
	}
	server_ = server;
	return true;
}

bool Client::Close()
{
	if (connection_)
		return connection_->Close();
	else
		return true;
}


} // namespace Mushroom
