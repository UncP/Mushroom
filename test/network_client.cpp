/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:34:58
**/

#include <unistd.h>
#include <string>

#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	Connection con1(EndPoint("127.0.0.1"), base.GetPoller());
	Signal::Register(SIGINT, [&base]() { base.Exit(); });
	if (!con1->Success()) {
		return 0;
	}

	con1->OnRead([con1]() {
		printf("read %u : %s\n", con1->GetInput().size(), con1->GetInput().data());
		usleep(500000);
		con1->Send(con1->GetInput());
	});
	con1->Send("hello world :)");
	base.Loop();
	return 0;
}
