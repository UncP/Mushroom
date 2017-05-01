/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-29 18:54:45
**/

#include "rpc_call.hpp"
#include "../src/rpc/rpc_client.hpp"

using namespace Mushroom;

int main()
{
	RpcClient client;

	client.OnConnect([](Connection *con) {
		con->OnSend([]() {
			printf("send\n");
		});
		con->OnRead([=]() {
			printf("read %u : %s\n", con->GetInput().size(), con->GetInput().data());
		});
	});

	if (!client.Connect(EndPoint("127.0.0.1")))
		return 0;

	Test test;
	Test::Pair args(1, 1);

	int32_t reply1, reply2;
	if (client.Call("Test.Add", &args, &reply1)) {
		test.Add(&args, &reply2);
		assert(reply1 == reply2);
	}

	if (client.Call("Test.Minus", &args, &reply1)) {
		test.Minus(&args, &reply2);
		assert(reply1 == reply2);
	}

	if (client.Call("Test.Mult", &args, &reply1)) {
		test.Mult(&args, &reply2);
		assert(reply1 == reply2);
	}

	float reply3, reply4;
	if (client.Call("Test.Div", &args, &reply3)) {
		test.Div(&args, &reply4);
		assert(reply3 == reply4);
	}

	client.Close();
	return 0;
}
