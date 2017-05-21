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
static Thread *loop;

class RaftTest
{
	public:
		RaftTest() { }

		static void MakeRaftSet(int number, float error_rate = 0.f) {
			base = new EventBase(4, 16);
			uint16_t port = 6000;
			rafts.clear();
			for (int i = 0; i < number; ++i)
				rafts.push_back(new RaftServer(base, port + i, i));
			for (int i = 0; i < number; ++i) {
				for (int j = 0; j < number; ++j) {
					if (i != j) {
						RpcConnection *con = new RpcConnection(EndPoint(port + j, "127.0.0.1"),
							base->GetPoller(), error_rate);
						rafts[i]->AddPeer(con);
					}
				}
			}
			loop = new Thread([&]() { base->Loop(); });
			loop->Start();
			for (auto e : rafts)
				e->Start();
		}

		static void FreeRaftSet() {
			for (auto e : rafts)
				e->Close();
			base->Exit();
			loop->Stop();
			for (auto e : rafts)
				delete e;
			delete base;
			delete loop;
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

// TEST(ElectionWithNoNetworkFaliure)
// {
// 	RaftTest::MakeRaftSet(5);
// 	uint32_t number;
// 	int32_t  id = -1;
// 	RaftTest::WaitForElection(1);
// 	RaftTest::CheckOneLeader(&number, &id);
// 	RaftTest::FreeRaftSet();
// 	ASSERT_TRUE(number == 1);
// 	rafts[id]->Status();
// }

// TEST(ElectionWithTotalNetworkFaliure)
// {
// 	RaftTest::MakeRaftSet(5, 1.0);
// 	uint32_t number;
// 	int32_t  id = -1;
// 	RaftTest::WaitForElection(2);
// 	RaftTest::CheckOneLeader(&number, &id);
// 	RaftTest::FreeRaftSet();
// 	ASSERT_TRUE(number == 0);
// }

TEST(ElectionWithPartialNetworkFaliure)
{
	RaftTest::MakeRaftSet(3, 0.5);
	uint32_t number;
	int32_t  id = -1;
	RaftTest::WaitForElection(2);
	RaftTest::CheckOneLeader(&number, &id);
	RaftTest::FreeRaftSet();
	ASSERT_TRUE(number == 0);
}

int main()
{
	Signal::Register(SIGINT, []() { RaftTest::FreeRaftSet(); });
	return RUN_ALL_TESTS();
}
