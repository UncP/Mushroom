/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-02 21:22:07
**/

#ifndef _RPC_CONNECTION_HPP_
#define _RPC_CONNECTION_HPP_

#include <map>

#include "../include/atomic.hpp"
#include "../include/spin_lock.hpp"
#include "../network/connection.hpp"
#include "rpc.hpp"
#include "marshaller.hpp"
#include "../network/channel.hpp"
#include "future.hpp"

namespace Mushroom {

class RpcConnection : public Connection
{
	public:
		static atomic_32_t RpcId;

		RpcConnection(const EndPoint &server, Poller *poller);

		RpcConnection(const Socket &socket, Poller *poller);

		~RpcConnection();

		template<typename T1, typename T2>
		inline Future* Call(const char *str, const T1 *args, T2 *reply);

		Marshaller& GetMarshaller();

		using Connection::OnRead;

	private:
		using Connection::Send;
		using Connection::OnWrite;

		SpinLock                     spin_;
		std::map<uint32_t, Future *> futures_;

		Marshaller marshaller_;
};

template<typename T1, typename T2>
inline Future* RpcConnection::Call(const char *str, const T1 *args, T2 *reply)
{
	uint32_t id  = RPC::Hash(str);
	uint32_t rid = RpcId++;
	Future *fu = new Future(rid);
	fu->CallBack([this, reply, fu]() {
		T2 tmp;
		marshaller_ >> tmp;
		if (!fu->timeout())
			*reply = tmp;
	});
	spin_.Lock();
	futures_.insert({rid, fu});
	spin_.Unlock();
	marshaller_.MarshalArgs(id, rid, args);
	SendOutput();
	return fu;
}

} // namespace Mushroom

#endif /* _RPC_CONNECTION_HPP_ */