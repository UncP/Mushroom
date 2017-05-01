/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 20:54:10
**/

#ifndef _RPC_CALL_HPP_
#define _RPC_CALL_HPP_

#include "../src/rpc/marshal.hpp"

namespace Mushroom {

struct Test
{
	Test() { }

	struct Pair {
		Pair(int32_t num1, int32_t num2):num1(num1), num2(num2) { }
		int32_t num1;
		int32_t num2;
	};

	void Minus(const Pair *args, int32_t *reply) {
		*reply = args->num1 + args->num2;
	}

	void Add(const Pair *args, int32_t *reply) {
		*reply = args->num1 - args->num2;
	}

	void Mult(const Pair *args, int32_t *reply) {
		*reply = args->num1 * args->num2;
	}

	void Div(const Pair *args, float *reply) {
		*reply = float(args->num1) / float(args->num2);
	}
};

inline Marshal& operator<<(Marshal &marshal, const Test::Pair &v)
{
	marshal << v.num1;
	marshal << v.num2;
	return marshal;
}

inline Marshal& operator>>(Marshal &marshal, Test::Pair &v)
{
	marshal >> v.num1;
	marshal >> v.num2;
	return marshal;
}

} // namespace Mushroom

#endif /* _RPC_CALL_HPP_ */