/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:35:06
**/

#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"
#include "../src/network/server.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	Signal::Register(SIGINT, [&] { base.Exit(); exit(0); });
	Server server(&base);
	server.Start();
	server.OnConnect([](Connection *con) {
		con->OnRead([con]() {
			printf("read %u : %s\n", con->GetInput().size(), con->GetInput().data());
			con->Send(con->GetInput());
		});
	});
	base.Loop();
	return 0;
}
