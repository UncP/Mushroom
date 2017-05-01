/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 10:16:19
**/

#ifndef _RPC_CLIENT_HPP_
#define _RPC_CLIENT_HPP_

#include <cassert>

#include "../network/endpoint.hpp"
#include "../network/function.hpp"
#include "../network/connection.hpp"
#include "marshal.hpp"

namespace Mushroom {

class RpcClient
{
	public:
		RpcClient();

		~RpcClient();

		void OnConnect(const ConnectCallBack &connectcb);

		bool Connect(const EndPoint &server);

		template<typename T1, typename T2>
		void Call(const char *str, const T1 *args, T2 *reply) {
			assert(connection_);
			connection_->GetOutput().Clear();
			*marshal_ <<  str;
			*marshal_ << *args;
			connection_->SendOutput();
		}

	private:
		Connection *connection_;
		Marshal    *marshal_;

		ConnectCallBack connectcb_;
};

} // namespace Mushroom

#endif /* _RPC_CLIENT_HPP_ */