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

namespace Mushroom {

class RpcConnection : private Connection
{
	public:
		RpcConnection(const EndPoint &server, Poller *poller);

		RpcConnection(const Socket &socket, Poller *poller);

		~RpcConnection();

		using Connection::Success;
		using Connection::OnRead;
		using Connection::OnWrite;
		using Connection::Close;

		template<typename T>
		inline bool Call(const char *str, const T *args);

	private:
		using Connection::channel_;
		Marshaller marshaller_;
};

template<typename T>
inline bool RpcConnection::Call(const char *str, const T *args)
{
	uint32_t id = RPC::Hash(str);
	marshaller_.Marshal(id, args);
	if (!channel_->CanWrite())
		channel_->EnableWrite(true);
	return true;
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */