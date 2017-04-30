/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-30 10:16:19
**/

#ifndef _RPC_CLIENT_HPP_
#define _RPC_CLIENT_HPP_

#include <string>
#include <unordered_map>

namespace Mushroom {

class RpcClient
{
	public:

		template<typename T1, typename T2, typename T3>
		void Register(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
			std::string key(str);
			assert(RPCs_.find(key) == RPCs_.end());
			RPCs_[key] = RPC(obj, fun);
		}

		template<typename T1, typename T2>
		void Call(const char *str, const T1 *args, T2 *reply) {
			auto p = RPCs_.find(std::string(str));
			assert(p != RPCs_.end());
			p->second(args, reply);
		}

	private:
		Marshal input_;
		Marshal output_;
		std::unordered_map<std::string, RPC> RPCs_;
};

} // namespace Mushroom

#endif /* _RPC_CLIENT_HPP_ */