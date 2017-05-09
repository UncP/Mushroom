/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:07
**/

#ifndef _RPC_CONNECTION_HPP_
#define _RPC_CONNECTION_HPP_

#include "../network/connection.hpp"
#include "rpc.hpp"
#include "marshaller.hpp"
#include "../network/channel.hpp"

namespace Mushroom {

class RpcConnection : public Connection
{
	public:
		RpcConnection(const EndPoint &server, Poller *poller);

		RpcConnection(const Socket &socket, Poller *poller);

		~RpcConnection();

		template<typename T>
		inline bool Call(const char *str, const T *args);

		Marshaller& Marshal();

	private:
		using Connection::Send;
		using Connection::SendOutput;

		Marshaller marshaller_;
};

template<typename T>
inline bool RpcConnection::Call(const char *str, const T *args)
{
	uint32_t id = RPC::Hash(str);
	marshaller_.MarshalArgs(id, args);
	if (!channel_->CanWrite())
		channel_->EnableWrite(true);
	return true;
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */