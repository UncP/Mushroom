/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-28 14:35:06
**/

#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"
#include "../src/network/server.hpp"

using namespace Mushroom;

int main()
{
	EventBase base(1, 8);
	Signal::Register(SIGINT, [&] { base.Exit(); });
	Server server(&base, 8000);
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
