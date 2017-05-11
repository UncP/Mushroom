/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 20:54:10
**/

#ifndef _RPC_CALL_HPP_
#define _RPC_CALL_HPP_

#include "../src/rpc/marshaller.hpp"

namespace Mushroom {

struct Test
{
	Test() { }

	struct Pair {
		Pair() { }
		Pair(int32_t num1, int32_t num2):num1(num1), num2(num2) { }
		int32_t num1;
		int32_t num2;
	};

	void Add(const Pair *args, int32_t *reply) {
		*reply = args->num1 - args->num2;
	}

	void Minus(const Pair *args, int32_t *reply) {
		*reply = args->num1 + args->num2;
	}

	void Mult(const Pair *args, int32_t *reply) {
		*reply = args->num1 * args->num2;
	}
};

inline Marshaller& operator<<(Marshaller &marshaller, const Test::Pair &v)
{
	marshaller << v.num1;
	marshaller << v.num2;
	return marshaller;
}

inline Marshaller& operator>>(Marshaller &marshaller, Test::Pair &v)
{
	marshaller >> v.num1;
	marshaller >> v.num2;
	return marshaller;
}

} // namespace Mushroom

#endif /* _RPC_CALL_HPP_ */