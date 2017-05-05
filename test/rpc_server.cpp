/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-01 19:52:00
**/

#include "rpc_call.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_server.hpp"

using namespace Mushroom;

int main()
{
	RpcServer server;
	Signal::Register(SIGINT, [&] { server.Close(); exit(0); });
	server.Start();
	server.Run();
	server.Close();
	return 0;
}
