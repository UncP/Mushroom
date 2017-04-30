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
		RpcClient();

		~RpcClient();

		template<typename T1, typename T2>
		void Call(const char *str, const T1 *args, T2 *reply) {

		}

	private:
		Marshal input_;
		Marshal output_;
};

} // namespace Mushroom

#endif /* _RPC_CLIENT_HPP_ */