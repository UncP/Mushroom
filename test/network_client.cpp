/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-04-28 14:34:58
**/

#include <unistd.h>

#include "../src/log/log.hpp"
#include "../src/network/signal.hpp"
#include "../src/network/eventbase.hpp"
#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	EventBase base(1, 8);
	Connection con(EndPoint(8000, "127.0.0.1"), base.GetPoller());
	Signal::Register(SIGINT, [&base]() { base.Exit(); });

	ExitIf(!con.Success(), "");

	con.OnRead([&con]() {
		printf("read %u : %s\n", con.GetInput().size(), con.GetInput().data());
		con.GetInput().Clear();
	});

	base.RunAfter(5000, [&base]() {
		base.Exit();
	});
	TimerId id1 = base.RunEvery(500, [&con]() {
		con.Send("hello world :)");
	});
	TimerId id2 = base.RunEvery(700, [&con]() {
		con.Send("hello world ;)");
	});
	base.RunAfter(1500, [&base, id1]() {
		base.Cancel(id1);
	});
	base.RunAfter(3500, [&base, id2]() {
		base.Cancel(id2);
	});
	base.Loop();
	return 0;
}
