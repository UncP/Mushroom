/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:35:06
**/

#include "../src/network/server.hpp"

using namespace Mushroom;

int main()
{
	Server server;
	server.Start();
	server.Run();
	server.OnRead([](Connection *con) {
		printf("read %u bytes :)\n", con->GetInput().size());
	});
	server.Close();
	return 0;
}
