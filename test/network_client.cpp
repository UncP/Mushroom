/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-28 14:34:58
**/

#include <unistd.h>
#include <string>

#include "../src/network/connection.hpp"

using namespace Mushroom;

int main()
{
	Connection *con = new Connection(EndPoint("127.0.0.1"));
	if (!con->Success()) {
		delete con;
		return 0;
	}

	con->OnRead([=]() {
		printf("read %u : %s\n", con->GetInput().size(), con->GetInput().data());
	});
	for (int i = 0; i < 10; ++i) {
		std::string msg = "hello world " + std::to_string(i);
		con->Send(msg.c_str(), msg.size());
	}
	sleep(2);
	con->HandleRead();
	con->Close();

	delete con;
	return 0;
}
