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

static EventBase *base = 0;
static Thread *loop = 0;
static vector<RaftServer *> rafts;
static vector<bool> connected;

namespace RaftTest {

static void FreeRaftSet() {
	for (auto e : rafts)
		e->Close();
	if (base) base->Exit();
	if (loop) loop->Stop();
	for (auto e : rafts)
		delete e;
	delete base;
	delete loop;
	base = 0;
	loop = 0;
	rafts.clear();
	connected.clear();
}

static void MakeRaftSet(int number, float error_rate = 0.f) {
	FreeRaftSet();
	base = new EventBase(8, 64);
	uint16_t port = 7000;
	for (int i = 0; i < number; ++i) {
		rafts.push_back(new RaftServer(base, port + i, i));
		connected.push_back(true);
	}
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
	connected[id] = false;
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
	connected[id] = true;
	for (auto e : rafts[id]->Peers())
		e->Enable();
	int32_t size = rafts.size();
	for (int32_t i = 0; i < size; ++i)
		if (id < i)
			rafts[i]->Peers()[id]->Enable();
		else if (id > i)
			rafts[i]->Peers()[id-1]->Enable();
}

static bool CommitAt(uint32_t index, uint32_t *commit, int *count)
{
	*count = 0;
	uint32_t pre = ~0;
	for (auto e : rafts) {
		if (!e->LogAt(index, commit)) continue;
		if (*count && pre != *commit) {
			printf("not match at %u, %u : %u\n", index, pre, *commit);
			return false;
		}
		pre = *commit;
		++*count;
	}
	return true;
}

static bool One(uint32_t number, int expect)
{
	for (int i = 0; i < 5; ++i) {
		for (uint32_t j = 0; j < rafts.size(); ++j) {
			if (!connected[j]) continue;
			uint32_t index;
			if (!rafts[j]->Start(number, &index)) continue;
			for (int k = 0; k < 4; ++k) {
				usleep(100 * 1000);
				uint32_t commit;
				int count;
				if (!CommitAt(index, &commit, &count))
					return false;
				if (count >= expect && commit == number)
					return true;
			}
		}
		usleep(100 * 1000);
	}
	return false;
}

} // namespace RaftTest

using namespace RaftTest;

// TEST(ElectionWithNoNetworkFaliure)
// {
// 	MakeRaftSet(5);
// 	int32_t number;
// 	int32_t id;
// 	CheckOneLeaderAfter(1, &number, &id);
// 	ASSERT_EQ(number,  1);
// 	rafts[id]->Status();
// }

// TEST(ElectionWithTotalNetworkFailure)
// {
// 	MakeRaftSet(5, 1.0);
// 	ASSERT_TRUE(CheckNoLeaderAfter(2));
// }
/*
TEST(ReelectionAfterNetworkFailure)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t leader1;
	CheckOneLeaderAfter(1, &number, &leader1);
	ASSERT_EQ(number,  1);
	rafts[leader1]->Status();

	DisableServer(leader1);
	int32_t leader2;
	CheckOneLeaderAfter(2, &number, &leader2);
	ASSERT_EQ(number,  1);
	rafts[leader2]->Status();

	EnableServer(leader1);
	int32_t leader3;
	CheckOneLeaderAfter(0.5, &number, &leader3);
	ASSERT_EQ(number,  1);
	ASSERT_EQ(leader2, leader3);

	DisableServer(leader2);
	DisableServer((leader2+1)%total);
	ASSERT_TRUE(CheckNoLeaderAfter(2));

	EnableServer((leader2+1)%total);
	int32_t leader4;
	CheckOneLeaderAfter(1, &number, &leader4);
	ASSERT_EQ(number, 1);
	rafts[leader4]->Status();

	EnableServer(leader2);
	int32_t leader5;
	CheckOneLeaderAfter(0.5, &number, &leader5);
	ASSERT_EQ(number, 1);
	ASSERT_EQ(leader4, leader5);
}
*/
TEST(AgreementWithoutNetworkFailure)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t id;
	CheckOneLeaderAfter(1, &number, &id);
	ASSERT_EQ(number, 1);
	rafts[leader1]->Status();

}

int main()
{
	Signal::Register(SIGINT, []() { FreeRaftSet(); });
	RUN_ALL_TESTS();
	FreeRaftSet();
	return 0;
}
