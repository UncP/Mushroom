/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 18:54:45
**/

#include <cassert>

#include "rpc_call.hpp"
#include "../src/log/log.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/network/eventbase.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	RpcConnection con(EndPoint("127.0.0.1"), base.GetPoller());
	ExitIf(!con.Success(), "");
	con.OnRead([&con]() {
		printf("read %u : %s\n", con.GetInput().size(), con.GetInput().data());
	});
	con.OnWrite([&con]() {
		printf("rpc call success\n");
	});

	Test test;
	Test::Pair args(1, 1);

	int32_t reply1, reply2, reply3;

	Future *fu1 = con.Call("Test.Add", &args, &reply1);
	Future *fu2 = con.Call("Test.Minus", &args, &reply2);
	Future *fu3 = con.Call("Test.Mult", &args, &reply3);

	fu1->Wait();
	fu2->Wait();
	fu3->Wait();

	int32_t reply4, reply5, reply6;
	test.Add(&args, &reply4);
	assert(reply1 == reply4);

	test.Minus(&args, &reply5);
	assert(reply2 == reply5);

	test.Mult(&args, &reply6);
	assert(reply3 == reply6);

	con.Close();
	return 0;
}
