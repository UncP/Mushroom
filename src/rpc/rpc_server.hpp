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

#include "marshal.hpp"
#include "rpc.hpp"
#include "../network/server.hpp"

namespace Mushroom {

class RpcServer : public Server
{
	public:
		RpcServer();

		~RpcServer();

		template<typename T1, typename T2, typename T3>
		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
			RPC rpc;
			rpc_t id = rpc.Generate(str, obj, fun);
			assert(RPCs_.find(id) == RPCs_.end());
			RPCs_[id] = rpc;
		}

		void Execute(rpc_t id, Marshal &marshal) {
			assert(RPCs_.find(id) != RPCs_.end());
			RPCs_[id](marshal);
		}

	private:
		std::unordered_map<rpc_t, RPC> RPCs_;
};

} // namespace Mushroom

#endif /* _RPC_SERVER_HPP_ */