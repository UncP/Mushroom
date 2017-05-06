/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 20:04:39
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

#include <functional>

#include "marshaller.hpp"

namespace Mushroom {

class RPC
{
	public:
		RPC();

		~RPC();

		template<typename T1, typename T2, typename T3>
		uint32_t Generate(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*));

		void operator()(Marshaller &marshaller);

		static uint32_t Hash(const char *str);

	private:
		std::function<void(Marshaller &)> service_;
};

template<typename T1, typename T2, typename T3>
uint32_t RPC::Generate(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
	service_ = [=](Marshaller &marshaller) {
		T2 args;
		marshaller >> args;
		T3 reply;
		(obj->*fun)(&args, &reply);
		marshaller << reply;
	};
	return Hash(str);
}

} // namespace Mushroom

#endif /* _RPC_HPP_ */