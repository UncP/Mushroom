/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:35:06
**/

#include "../src/network/signal.hpp"
#include "../src/network/connection.hpp"
#include "../src/network/server.hpp"

using namespace Mushroom;

int main()
{
	Server server;
	Signal::Register(SIGINT, [&] { server.Close(); exit(0); });
	server.Start();
	server.OnConnect([](Connection *con) {
		con->OnSend([con](uint32_t sent) {
			printf("send %u bytes\n", sent);
		});
		con->OnRead([con]() {
			printf("read %u bytes\n", con->GetInput().size());
			con->Send(con->GetInput());
		});
	});
	server.Run();
	server.Close();
	return 0;
}
