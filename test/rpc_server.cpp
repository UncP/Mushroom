/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 19:52:00
**/

#include "rpc_call.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_server.hpp"
#include "../src/network/eventbase.hpp"

using namespace Mushroom;

int main()
{
	EventBase base(1, 8);
	RpcServer server(&base, 7000);
	Signal::Register(SIGINT, [&] { base.Exit(); });
	Test test;
	server.Register("Test::Add", &test, &Test::Add);
	server.Register("Test::Minus", &test, &Test::Minus);
	server.Register("Test::Mult", &test, &Test::Mult);
	server.Start();
	base.Loop();
	return 0;
}
