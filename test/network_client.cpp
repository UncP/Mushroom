/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:34:58
**/

#include <unistd.h>

#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	Connection con(EndPoint("127.0.0.1"), base.GetPoller());
	Signal::Register(SIGINT, [&base]() { base.Exit(); });

	ExifIf(!con.Success(), "");

	con->OnRead([con]() {
		printf("read %u : %s\n", con->GetInput().size(), con->GetInput().data());
		usleep(500000);
		con->Send(con->GetInput());
	});
	con->Send("hello world :)");

	base.Loop();
	return 0;
}
