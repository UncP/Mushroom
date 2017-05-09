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
	EventBase base;
	RpcServer server(&base);
	Signal::Register(SIGINT, [&] { base.Exit(); exit(0); });
	server.Start();
	return 0;
}
