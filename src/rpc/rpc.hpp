/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-22 21:22:47
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

#include <functional>

namespace Mushroom {

template<typename T1, typename T2, typename T3>
class RPC
{
	public:
		RPC(T1 *obj, void (T1::*(fun))(const T2 *, T3 *)) {

		}

	private:
		std::function<void()> service_;
};

} // namespace Mushroom

#endif /* _RPC_HPP_ */