/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 11:23:02
**/

#ifndef _RPC_SERVER_HPP_
#define _RPC_SERVER_HPP_

#include <cassert>
#include <unordered_map>

#include "../log/log.hpp"
#include "../network/server.hpp"
#include "utility.hpp"
#include "rpc.hpp"

namespace Mushroom {

class RpcServer : public Server
{
	public:
		RpcServer();

		~RpcServer();

		template<typename T1, typename T2, typename T3>
		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*));

	private:

		std::unordered_map<rpc_t, RPC> services_;

		void HandleAccept();
};

template<typename T1, typename T2, typename T3>
void RpcServer::Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*))
{
	RPC service;
	rpc_t id = service.Generate(str, obj);
	FatalIf(services_.find(id) != services_.end(), "service %u existed :(", id);
	services_.insert({id, service});
}

} // namespace Mushroom

#endif /* _RPC_SERVER_HPP_ */