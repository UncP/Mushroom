/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 18:54:45
**/

#include <cassert>
#include <vector>

#include "rpc_call.hpp"
#include "../src/log/log.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/include/thread.hpp"

using namespace Mushroom;

int main(int argc, char **argv)
{
	EventBase base(1, 8);
	RpcConnection con(EndPoint(7000, "127.0.0.1"), base.GetPoller());

	ExitIf(!con.Success(), "");

	Thread loop([&]() {
		base.Loop();
	});

	loop.Start();

	Test test;
	Test::Pair args(2047, 65535);

	int total = (argc == 2) ? atoi(argv[1]) : 1;
	std::vector<int32_t> reply(total);
	std::vector<Future *> futures;
	futures.reserve(total);
	for (int i = 0; i < total; ++i)
		futures.push_back(con.Call("Test::Add", &args, &reply[i]));

	Signal::Register(SIGINT, [&base, &futures]() {
		base.Exit();
		for (auto e : futures)
			if (!e->ok())
				e->Cancel();
	});

	for (auto e : futures)
		e->Wait();

	con.Close();
	base.Exit();

	int32_t reply2;
	test.Add(&args, &reply2);
	int success = 0, failure = 0, bad = 0;
	for (int i = 0; i < total; ++i) {
		if (futures[i]->ok()) {
			if (reply2 == reply[i])
				++success;
			else
				++failure;
		} else {
			++bad;
		}
	}

	printf("\033[33mtotal  : %d\033[0m\n", total);
	printf("\033[32msuccess: %d\033[0m\n", success);
	printf("\033[31mfailure: %d\033[0m\n", failure);
	printf("\033[34mbad    : %d\033[0m\n", bad);

	loop.Stop();
	return 0;
}
