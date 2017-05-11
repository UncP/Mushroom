/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 18:54:45
**/

#include <cassert>

#include "rpc_call.hpp"
#include "../src/log/log.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/include/thread.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	RpcConnection con(EndPoint("127.0.0.1"), base.GetPoller());

	ExitIf(!con.Success(), "");

	Thread loop([&]() {
		base.Loop();
	});

	loop.Start();

	Test test;
	Test::Pair args(3, 4);

	int32_t reply1, reply2, reply3;

	Future *fu1 = con.Call("Test::Add", &args, &reply1);
	Future *fu2 = con.Call("Test::Minus", &args, &reply2);
	Future *fu3 = con.Call("Test::Mult", &args, &reply3);

	Signal::Register(SIGINT, [&base, fu1, fu2, fu3]() {
		base.Exit(); fu1->Abandon(); fu2->Abandon(), fu3->Abandon();
	});

	fu1->Wait();
	fu2->Wait();
	fu3->Wait();

	base.Exit();
	loop.Stop();
	con.Close();

	int32_t reply4, reply5, reply6;
	test.Add(&args, &reply4);
	assert(reply1 == reply4);

	test.Minus(&args, &reply5);
	assert(reply2 == reply5);

	test.Mult(&args, &reply6);
	assert(reply3 == reply6);

	return 0;
}
