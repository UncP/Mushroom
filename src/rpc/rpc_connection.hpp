/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:07
**/

#ifndef _RPC_CONNECTION_HPP_
#define _RPC_CONNECTION_HPP_

#include "../network/connection.hpp"
#include "utility.hpp"
#include "rpc.hpp"
#include "marshaller.hpp"

namespace Mushroom {

class RpcConnection : public Connection
{
	public:
		RpcConnection(const EndPoint &server);

		~RpcConnection();

		template<typename T1, typename T2>
		bool Call(const char *str, const T1 *args, T2 *reply);

	private:
		Marshaller marshaller_;
};

template<typename T1, typename T2>
bool RpcConnection::Call(const char *str, const T1 *args, T2 *reply)
{
	output_.Clear();
	rpc_t id = RPC::Hash(str);
	marshaller_.Marshal(id, args);
	SendOutput();
	return true;
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */