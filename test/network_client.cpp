/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:34:58
**/

#include <unistd.h>

#include "../src/log/log.hpp"
#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	EventBase base;
	Connection con(EndPoint("127.0.0.1"), base.GetPoller());
	Signal::Register(SIGINT, [&base]() { base.Exit(); });

	ExitIf(!con.Success(), "");

	con.OnRead([&con]() {
		printf("read %u : %s\n", con.GetInput().size(), con.GetInput().data());
		con.GetInput().Clear();
	});

	base.RunAfter(5000, [&base]() {
		base.Exit();
	});
	TimerId id = base.RunEvery(500, [&con]() {
		con.Send("hello world :)");
	});
	base.RunAfter(1500, [&base, id]() {
		base.Cancel(id);
	});
	base.Loop();
	return 0;
}
