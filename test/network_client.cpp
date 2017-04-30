/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:34:58
**/

#include <unistd.h>

#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	Connection *con = new Connection(EndPoint("127.0.0.1"));
	if (!con->Success()) return 0;

	con->OnSend([&](uint32_t sent) {
		printf("send %u bytes\n", sent);
	});
	con->OnRead([&]() {
		printf("read %u bytes\n", con->GetInput().size());
	});
	con->Send("hello world :)");

	sleep(2);
	con->HandleRead();
	printf("%s\n", con->GetInput().begin());
	con->Close();

	delete con;
	return 0;
}
