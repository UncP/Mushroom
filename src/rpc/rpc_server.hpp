/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 11:23:02
**/

#ifndef _RPC_SERVER_HPP_
#define _RPC_SERVER_HPP_

#include <string>
#include <unordered_map>

#include "rpc.hpp"

namespace Mushroom {

class RpcServer
{
	public:
		RpcServer();

		~RpcServer();

		template<typename T1, typename T2, typename T3>
		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
			std::string key(str);
			assert(RPCs_.find(key) == RPCs_.end());
		}

	private:
		std::unordered_map<std::string, RPC> RPCs_;
};

} // namespace Mushroom

#endif /* _RPC_SERVER_HPP_ */