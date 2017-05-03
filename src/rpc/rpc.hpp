/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 20:04:39
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

#include <functional>

#include "utility.hpp"
#include "marshaller.hpp"

namespace Mushroom {

class RPC
{
	public:
		RPC() { }

		~RPC() { }

		template<typename T1, typename T2, typename T3>
		rpc_t Generate(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
			service_ = [=](Marshaller &marshaller) {
				T2 args;
				marshaller >> args;
				T3 reply;
				(obj->*fun)(&args, &reply);
				marshaller << reply;
			};
			return Hash(str);
		}

		inline void operator()(Marshaller &marshaller) {
			service_(marshaller);
		}

		static rpc_t Hash(const char *str) {
			rpc_t ret = 0;
			char *p = (char *)str;
			while (p)
				ret += rpc_t(*p++);
			return ret;
		}

	private:
		std::function<void(Marshaller &)> service_;
};

} // namespace Mushroom

#endif /* _RPC_HPP_ */