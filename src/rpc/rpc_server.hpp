/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-30 11:23:02
**/

#ifndef _RPC_SERVER_HPP_
#define _RPC_SERVER_HPP_

#include <cassert>
#include <unordered_map>

#include "../include/utility.hpp"
#include "../include/atomic.hpp"
#include "../network/server.hpp"
#include "rpc.hpp"

namespace Mushroom {

class EventBase;

class RpcServer : public Server
{
	public:
		RpcServer(EventBase *event_base, uint16_t port);

		virtual ~RpcServer();

		void Start();

		void Close();

		template<typename T1, typename T2, typename T3>
		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*));

		uint32_t RpcCount();

	private:
		std::unordered_map<uint32_t, RPC *> services_;

		atomic_32_t rpc_count_;

		void HandleAccept();
};

template<typename T1, typename T2, typename T3>
void RpcServer::Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*))
{
	RPC *rpc = new RPC();
	uint32_t id = rpc->Generate(str, obj, fun);
	assert(services_.find(id) == services_.end());
	services_.insert({id, rpc});
}

} // namespace Mushroom

#endif /* _RPC_SERVER_HPP_ */