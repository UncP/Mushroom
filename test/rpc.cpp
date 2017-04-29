/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 18:54:45
**/

#include "../src/rpc/rpc.hpp"

using namespace Mushroom;

struct obj {
	obj() { }

	void fun(const int *, int *) {

	}
};

int main()
{
	RPC<obj, int, int> rpc;
	obj o;
	rpc.Register("obj::fun", &o, &obj::fun);
	return 0;
}