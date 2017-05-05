/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-05 11:15:09
**/

#include "rpc.hpp"

namespace Mushroom {

RPC::RPC() { }

RPC::~RPC() { }

void RPC::operator()(Marshaller &marshaller) {
	service_(marshaller);
}

rpc_t RPC::Hash(const char *str) {
	rpc_t ret = 0;
	char *p = (char *)str;
	while (p)
		ret += rpc_t(*p++);
	return ret;
}

} // namespace Mushroom

