/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-01 20:04:39
**/

#ifndef _RPC_HPP_
#define _RPC_HPP_

#include "../include/utility.hpp"
#include "marshaller.hpp"

namespace Mushroom {

class RPC
{
	public:
		RPC():service_(0) { }

		template<typename T1, typename T2, typename T3>
		inline uint32_t Generate(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*));

		inline void GetReady(Marshaller &marshaller) { marshaller_ = marshaller; }

		inline void operator()() { service_(); }

		inline static uint32_t Hash(const char *str);

	private:
		Marshaller marshaller_;
		Func       service_;
};

template<typename T1, typename T2, typename T3>
inline uint32_t RPC::Generate(const char *str, T1 *obj, void (T1::*(fun))(const T2*, T3*)) {
	service_ = [this, obj, fun]() {
		T2 *args;
		uint32_t rid = marshaller_.UnmarshalArgs(&args);
		T3 reply;
		(obj->*fun)(args, &reply);
		marshaller_.MarshalReply(rid, &reply);
	};
	return Hash(str);
}

inline uint32_t RPC::Hash(const char *str) {
	uint32_t ret = 0;
	char *p = (char *)str;
	while (*p)
		ret += uint32_t(*p++);
	return ret;
}

} // namespace Mushroom

#endif /* _RPC_HPP_ */