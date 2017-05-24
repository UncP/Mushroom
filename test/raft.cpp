/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-08 14:22:46
**/

#include <unistd.h>
#include <unit.h>
#include <map>

#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/raft/raft_server.hpp"
#include "../src/include/thread.hpp"

using namespace std;
using namespace Mushroom;

static EventBase *base;
static vector<RaftServer *> rafts;
static Thread *loop;

namespace RaftTest {

static void MakeRaftSet(int number, float error_rate = 0.f) {
	base = new EventBase(8, 32);
	uint16_t port = 7000;
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
		e->RescheduleElection();
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

static void WaitForElection(float factor) {
	usleep(factor * RaftServer::ElectionTimeout * 1000);
}

static void CheckOneLeaderAfter(float factor, int32_t *number, int32_t *id) {
	WaitForElection(factor);
	map<uint32_t, vector<int32_t>> map;
	*id = -1;
	*number = 0;
	uint32_t last = 0;
	for (auto e : rafts) {
		uint32_t now;
		if (e->IsLeader(&now))
			map[now].push_back(e->Id());
		last = last < now ? now : last;
	}
	if (map.size()) {
		auto &leaders = map[last];
		*number = leaders.size();
		if (*number == 1)
			*id = leaders[0];
	}
}

static bool CheckNoLeaderAfter(float factor) {
	int32_t number;
	int32_t  id;
	CheckOneLeaderAfter(factor, &number, &id);
	return number == 0;
}

static void DisableServer(int32_t id) {
	for (auto e : rafts[id]->Peers())
		e->Disable();
	int32_t size = rafts.size();
	for (int32_t i = 0; i < size; ++i)
		if (id < i)
			rafts[i]->Peers()[id]->Disable();
		else if (id > i)
			rafts[i]->Peers()[id-1]->Disable();
}

static void EnableServer(int32_t id) {
	for (auto e : rafts[id]->Peers())
		e->Enable();
	int32_t size = rafts.size();
	for (int32_t i = 0; i < size; ++i)
		if (id < i)
			rafts[i]->Peers()[id]->Enable();
		else if (id > i)
			rafts[i]->Peers()[id-1]->Enable();
}

} // namespace RaftTest

using namespace RaftTest;
/*
TEST(ElectionWithNoNetworkFaliure)
{
	MakeRaftSet(5);
	int32_t number;
	int32_t  id;
	CheckOneLeaderAfter(1, &number, &id);
	FreeRaftSet();
	ASSERT_TRUE(number == 1);
	rafts[id]->Status();
}

TEST(ElectionWithTotalNetworkFailure)
{
	MakeRaftSet(5, 1.0);
	int32_t number;
	int32_t  id;
	CheckOneLeaderAfter(2, &number, &id);
	FreeRaftSet();
	ASSERT_TRUE(number == 0);
}
*/
TEST(RelectionAfterNetworkFailure)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t leader1;
	CheckOneLeaderAfter(1, &number, &leader1);
	EXPECT_TRUE(number == 1);
	rafts[leader1]->Status();

	DisableServer(leader1);
	int32_t leader2;
	CheckOneLeaderAfter(2, &number, &leader2);
	EXPECT_EQ(number, 1);
	if (number == 1)
		rafts[leader2]->Status();

	EnableServer(leader1);
	int32_t leader3;
	CheckOneLeaderAfter(0.5, &number, &leader3);
	EXPECT_TRUE(number == 1);
	EXPECT_TRUE(leader2 == leader3);

	// DisableServer(leader2);
	// DisableServer((leader2+1)%total);
	// EXPECT_TRUE(CheckNoLeaderAfter(2));

	// EnableServer((leader2+1)%total);
	// int32_t leader4;
	// CheckOneLeaderAfter(1, &number, &leader4);
	// EXPECT_TRUE(number == 1);

	// EnableServer(leader2);
	// int32_t leader5;
	// CheckOneLeaderAfter(0.5, &number, &leader5);
	// EXPECT_TRUE(number == 1);
	// EXPECT_TRUE(leader4 == leader5);

	FreeRaftSet();
}

int main()
{
	Signal::Register(SIGINT, []() { FreeRaftSet(); });
	return RUN_ALL_TESTS();
}
