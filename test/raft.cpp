/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-08 14:22:46
**/

#include <unistd.h>
#include <unit.h>

#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/raft/raft_server.hpp"
#include "../src/include/thread.hpp"

using namespace std;
using namespace Mushroom;

static EventBase *base;
static vector<RaftServer *> rafts;

class RaftTest
{
	public:
		RaftTest() { }

		static void MakeRaftSet(int number) {
			if (rafts.size())
				FreeRaftSet();
			base = new EventBase(4, 16);
			rafts.reserve(number);
			uint16_t port = 7000;
			for (int i = 0; i < number; ++i)
				rafts.push_back(new RaftServer(base, port + i, i));
			for (int i = 0; i < number; ++i) {
				for (int j = 0; j < number; ++j) {
					if (i != j) {
						RpcConnection *con = new RpcConnection(EndPoint(port + j, "127.0.0.1"),
							base->GetPoller());
						rafts[i]->AddPeer(con);
					}
				}
			}
		}

		static void FreeRaftSet() {
			for (auto e : rafts)
				delete e;
			rafts.clear();
			if (base) {
				base->Exit();
			}
		}

		static void CheckOneLeader(uint32_t *number, int32_t *id) {
			*number = 0;
			for (auto e : rafts) {
				if (e->IsLeader()) {
					*id = e->Id();
					++*number;
				}
			}
		}

		static bool CheckNoLeader() {
			for (auto e : rafts) {
				if (e->IsLeader())
					return false;
			}
			return true;
		}

		static void WaitForTimeout(float factor) {
			usleep(factor * RaftServer::TimeoutTop * 1000);
		}

		static void WaitForElection(float factor) {
			usleep(factor * RaftServer::ElectionTimeout * 1000);
		}
};

TEST(ElectionWithNoNetworkFaliure)
{
	RaftTest::MakeRaftSet(3);
	for (auto e : rafts)
		e->Start();
	uint32_t number;
	int32_t  id = -1;
	Thread thread([&]() {
		base->Loop();
		delete base;
		base = 0;
	});
	thread.Start();
	RaftTest::WaitForElection(0.5);
	RaftTest::CheckOneLeader(&number, &id);
	RaftTest::FreeRaftSet();
	thread.Stop();
	ASSERT_TRUE(number == 1);
	rafts[id]->Status();
}

int main()
{
	Signal::Register(SIGINT, [&]() {
		printf("signal captured ;)\n");
		RaftTest::FreeRaftSet();
	});
	return RUN_ALL_TESTS();
}
