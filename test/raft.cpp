/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:  2017-05-08 14:22:46
**/

#include <unistd.h>
#include <map>
#include <random>
#include <ctime>

#include "unit.h"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/raft/raft_server.hpp"
#include "../src/include/thread.hpp"
#include "../src/network/time.hpp"

using namespace std;
using namespace Mushroom;

static EventBase *base = 0;
static Thread *loop = 0;
static vector<RaftServer *> rafts;
static vector<bool> connected;
static uint16_t port_base = 7000;

namespace RaftTest {

static void StartServer(uint32_t idx)
{
	rafts[idx] = new RaftServer(base, port_base++, idx);
}

static void ConnectServer(uint32_t idx)
{
	for (uint32_t i = 0; i < rafts.size(); ++i) {
		if (idx == i) continue;
		rafts[idx]->AddPeer(new RpcConnection(EndPoint(rafts[i]->Port(), "127.0.0.1"),
			base->GetPoller(), 0));
	}
	connected[idx] = true;
}

static void CrashServer(uint32_t idx)
{
	rafts[idx]->Close();
	delete rafts[idx];
	rafts[idx] = 0;
}

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

static void MakeRaftSet(int total) {
	FreeRaftSet();
	base = new EventBase(4, 64);
	rafts.resize(total);
	connected.resize(total);
	for (int i = 0; i < total; ++i)
		StartServer(i);
	for (int i = 0; i < total; ++i)
		ConnectServer(i);

	loop = new Thread([&]() { base->Loop(); });
	loop->Start();
	for (auto e : rafts)
		e->RescheduleElection();
}

static void WaitForElection(float factor) {
	if (factor == 0.f)
		return ;
	usleep(factor * RaftServer::ElectionTimeout * 1000);
}

static void CheckOneLeaderAfter(float factor, int32_t *number, int32_t *id) {
	WaitForElection(factor);
	map<uint32_t, vector<int32_t>> map;
	*id = -1;
	*number = 0;
	uint32_t last = 0;
	for (uint32_t i = 0; i < rafts.size(); ++i) {
		if (!connected[i])
			continue;
		uint32_t term;
		if (rafts[i]->IsLeader(&term))
			map[term].push_back(rafts[i]->Id());
		last = last < term ? term : last;
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
	for (auto e : rafts[id]->Connections())
		((RpcConnection *)e)->Disable();
}

static void EnableServer(int32_t id) {
	connected[id] = true;
	for (auto e : rafts[id]->Peers())
		e->Enable();
	for (auto e : rafts[id]->Connections())
		((RpcConnection *)e)->Enable();
}

static void DisableServerFor(float factor, int32_t id) {
	DisableServer(id);
	WaitForElection(factor);
	EnableServer(id);
}

static bool CommittedAt(uint32_t index, uint32_t *commit, int *count)
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
		*count += 1;
	}
	return true;
}

static uint32_t One(uint32_t number, int expect)
{
	int count;
	int64_t now = Time::Now();
	for (; Time::Now() < (now + 5000);) {
		uint32_t index = ~0u;
		for (uint32_t j = 0; j < rafts.size(); ++j) {
			if (!connected[j]) continue;
			if (rafts[j]->Start(number, &index))
				break;
		}
		if (index == ~0u) {
			usleep(100 * 1000);
			continue;
		}
		for (int k = 0; k < 20; ++k) {
			usleep(50 * 1000);
			uint32_t commit;
			if (!CommittedAt(index, &commit, &count))
				continue;
			if (count >= expect && commit == number)
				return index;
		}
	}
	printf("%u failed to reach agreement, %d : %d\n", number, expect, count);
	return ~0u;
}

static void PrintAllServer(bool print_log = true, bool print_next = true)
{
	for (auto e : rafts)
		e->Status(print_log, print_next);
}

static void PrintRaftServer(int32_t index)
{
	rafts[index]->Status(true, false);
}

static uint32_t RpcCount()
{
	uint32_t ret = 0;
	for (auto e : rafts)
		ret += e->RpcCount();
	return ret;
}

static uint32_t Wait(uint32_t index, int expect, uint32_t term)
{
	uint32_t to = 10;
	uint32_t commit;
	int count;
	for (int i = 0; i < 10; ++i) {
		if (!CommittedAt(index, &commit, &count))
			continue;
		if (count >= expect)
			break;
		usleep(to * 1000);
		if (to < 1000) to *= 2;
		for (auto e : rafts)
			if (e->Term() > term)
				return ~0;
	}
	CommittedAt(index, &commit, &count);
	if (count < expect)
		return ~0;
	return commit;
}

} // namespace RaftTest

using namespace RaftTest;

TEST(ElectionWithNoNetworkFaliure)
{
	MakeRaftSet(5);
	int32_t number;
	int32_t id;
	CheckOneLeaderAfter(1, &number, &id);
	ASSERT_EQ(number,  1);
}


TEST(ReelectionAfterNetworkFailure)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t leader1;
	CheckOneLeaderAfter(1, &number, &leader1);
	ASSERT_EQ(number, 1);

	DisableServer(leader1);
	WaitForElection(1);
	int32_t leader2;
	CheckOneLeaderAfter(2, &number, &leader2);
	ASSERT_EQ(number, 1);

	EnableServer(leader1);
	int32_t leader3;
	CheckOneLeaderAfter(0.5, &number, &leader3);
	ASSERT_EQ(number, 1);
	ASSERT_EQ(leader2, leader3);

	DisableServer(leader2);
	DisableServer((leader2+1)%total);
	ASSERT_TRUE(CheckNoLeaderAfter(1));

	EnableServer((leader2+1)%total);
	int32_t leader4;
	CheckOneLeaderAfter(2, &number, &leader4);
	ASSERT_EQ(number, 1);

	EnableServer(leader2);
	int32_t leader5;
	CheckOneLeaderAfter(1, &number, &leader5);
	ASSERT_EQ(number, 1);
	ASSERT_EQ(leader4, leader5);
}

/*
TEST(AgreementWithoutNetworkFailure)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t id;
	CheckOneLeaderAfter(1, &number, &id);
	ASSERT_EQ(number, 1);

	for (uint32_t i = 0; i < 5; ++i) {
		uint32_t commit;
		int count;
		ASSERT_TRUE(CommittedAt(i, &commit, &count));
		ASSERT_EQ(count, 0);
		uint32_t index = One(i, total);
		ASSERT_EQ(index, i);
	}
}


TEST(AgreementWithFollowerDisconnected)
{
	uint32_t total = 3;
	MakeRaftSet(total);
	int32_t number;
	int32_t leader;
	CheckOneLeaderAfter(1, &number, &leader);
	ASSERT_EQ(number, 1);

	uint32_t lg  = 0;
	uint32_t idx = 0;
	ASSERT_EQ(idx++, One(lg++, total));

	DisableServer((leader+1)%total);
	ASSERT_EQ(idx++, One(lg++, total-1));
	ASSERT_EQ(idx++, One(lg++, total-1));

	DisableServerFor(1, leader);
	int32_t leader2;
	CheckOneLeaderAfter(2, &number, &leader2);
	ASSERT_EQ(number, 1);

	ASSERT_EQ(idx++, One(lg++, total-1));
	ASSERT_EQ(idx++, One(lg++, total-1));

	EnableServer((leader+1)%total);
	WaitForElection(0.5);
	ASSERT_EQ(idx++, One(lg++, total));

	DisableServerFor(1, leader2);
	int32_t leader3;
	CheckOneLeaderAfter(2, &number, &leader3);
	ASSERT_EQ(number, 1);
	ASSERT_EQ(idx++, One(lg++, total));
}


TEST(AgreementWithHalfFollowerDisconnected)
{
	uint32_t total = 5;
	MakeRaftSet(total);

	uint32_t lg  = 0;
	uint32_t idx = 0;
	ASSERT_EQ(idx++, One(lg++, total));

	int32_t number;
	int32_t leader;
	CheckOneLeaderAfter(1, &number, &leader);
	ASSERT_EQ(number, 1);

	DisableServer((leader+1)%total);
	DisableServer((leader+2)%total);
	DisableServer((leader+3)%total);

	uint32_t index;
	ASSERT_TRUE(rafts[leader]->Start(lg++, &index));
	ASSERT_EQ(index, 1u);

	WaitForElection(1);
	uint32_t commit;
	int count;
	CommittedAt(index, &commit, &count);
	ASSERT_EQ(count, 0);

	EnableServer((leader+1)%total);
	EnableServer((leader+2)%total);
	EnableServer((leader+3)%total);

	int32_t leader2;
	CheckOneLeaderAfter(2, &number, &leader2);
	ASSERT_EQ(number, 1);

	uint32_t index2;
	ASSERT_TRUE(rafts[leader2]->Start(lg++, &index2));
	ASSERT_TRUE(index2 >= 1 && index2 < 3);

	ASSERT_NE(One(lg++, total), ~0u);
	WaitForElection(1);
	ASSERT_NE(One(lg++, total), ~0u);
}


TEST(RejoinOfPartitionedLeader)
{
	uint32_t total = 3;
	MakeRaftSet(total);

	uint32_t lg  = 0;
	uint32_t idx = 0;
	ASSERT_EQ(idx++, One(lg++, total));

	int32_t number;
	int32_t leader;
	CheckOneLeaderAfter(0, &number, &leader);
	ASSERT_EQ(number, 1);

	DisableServer(leader);

	uint32_t index;
	ASSERT_TRUE(rafts[leader]->Start(lg++, &index));
	ASSERT_TRUE(rafts[leader]->Start(lg++, &index));
	ASSERT_TRUE(rafts[leader]->Start(lg++, &index));

	WaitForElection(1);
	ASSERT_EQ(idx++, One(lg++, total-1));

	int32_t leader2;
	CheckOneLeaderAfter(0, &number, &leader2);
	ASSERT_EQ(number, 1);

	DisableServer(leader2);
	EnableServer(leader);
	WaitForElection(2);
	ASSERT_EQ(idx++, One(lg++, total-1));

	EnableServer(leader2);
	ASSERT_EQ(idx++, One(lg++, total));
}

TEST(LeaderBackupIncorrectLog)
{
	uint32_t total = 5;
	MakeRaftSet(total);

	uint32_t lg  = 0;
	ASSERT_NE(One(lg++, total), ~0u);

	int32_t number;
	int32_t leader;
	CheckOneLeaderAfter(0, &number, &leader);
	ASSERT_EQ(number, 1);

	DisableServer((leader+2)%total);
	DisableServer((leader+3)%total);
	DisableServer((leader+4)%total);

	int all = 100;
	uint32_t index;
	for (int i = 0; i < all; ++i)
		ASSERT_TRUE(rafts[leader]->Start(lg++, &index));

	DisableServer((leader+0)%total);
	DisableServer((leader+1)%total);

	EnableServer((leader+2)%total);
	EnableServer((leader+3)%total);
	EnableServer((leader+4)%total);

	for (int i = 0; i < all; ++i)
		ASSERT_NE(One(lg++, total-2), ~0u);

	int32_t leader2;
	CheckOneLeaderAfter(0, &number, &leader2);
	ASSERT_EQ(number, 1);

	int32_t other = (leader + 2) % total;
	if (other == leader2)
		other = (leader2 + 1) % total;
	DisableServer(other);

	for (int i = 0; i < all; ++i)
		ASSERT_TRUE(rafts[leader2]->Start(lg++, &index));
	WaitForElection(0.5);

	for (int i = 0; i < int(total); ++i)
		DisableServer(i);
	EnableServer((leader+0)%total);
	EnableServer((leader+1)%total);
	EnableServer(other);

	WaitForElection(2);
	for (int i = 0; i < all; ++i)
		ASSERT_NE(One(lg++, total-2), ~0u);

	for (int i = 0; i < int(total); ++i)
		EnableServer(i);

	ASSERT_NE(One(lg++, total), ~0u);
}


TEST(RpcCount)
{
	uint32_t total = 3;
	MakeRaftSet(total);

	int32_t number;
	int32_t leader;
	CheckOneLeaderAfter(0.5, &number, &leader);
	ASSERT_EQ(number, 1);
	uint32_t count = RpcCount();
	ASSERT_LT(count, 30u);

	uint32_t lg  = 0;
	uint32_t all1, all2;
loop:
	for (int i = 0; i < 3; ++i) {
		if (i) WaitForElection(1);
		all1 = RpcCount();
		uint32_t index, term;
		ASSERT_TRUE(rafts[leader]->Start(lg++, &index));
		term = rafts[leader]->Term();
		int iter = 10;
		vector<uint32_t> logs;
		for (int j = 1; j < iter+2; ++j) {
			uint32_t tmp_idx;
			logs.push_back(lg);
			if (!rafts[leader]->Start(lg++, &tmp_idx))
				goto loop;
			uint32_t tmp_trm = rafts[leader]->Term();
			if (tmp_trm != term)
				goto loop;
			ASSERT_EQ(index+j, tmp_idx);
		}
		for (int j = 1; j < iter+1; ++j) {
			uint32_t number = Wait(index+j, total, term);
			if (number == ~0u)
				goto loop;
			ASSERT_EQ(number, logs[j-1]);
		}
		all2 = 0;
		for (auto e : rafts) {
			if (e->Term() != term)
				goto loop;
			all2 += e->RpcCount();
		}
		ASSERT_LE(int(all2-all1), (iter+1+3)*3);
		break;
	}

	WaitForElection(1);
	uint32_t all3 = RpcCount();
	ASSERT_LE(all3 - all2, 90u);
}
*/
/*
TEST(Figure8)
{
	uint32_t total = 5;
	uint32_t up = total;
	MakeRaftSet(total);
	std::default_random_engine eng(time(0));
	std::uniform_int_distribution<int> dist(0, 999);
	uint32_t iter = 10;
	for (uint32_t i = 0; i < iter; ++i) {
		int32_t leader = -1;
		for (uint32_t j = 0; j < total; ++j) {
			if (!rafts[j]) continue;
			uint32_t index;
			if (rafts[j]->Start(i, &index)) {
				leader = int32_t(j);
				break;
			}
		}
		if (dist(eng) < 100)
			usleep((dist(eng) % (RaftServer::ElectionTimeout / 2)) * 1000);
		else
			usleep((dist(eng) % 13) * 1000);

		if (leader != -1) {
			CrashServer(leader);
			--up;
		}

		if (up < ((total + 1) / 2)) {
			int idx = dist(eng) % total;
			if (!rafts[idx]) {
				StartServer(idx);
				ConnectServer(idx);
				++up;
			}
		}
	}

	for (uint32_t i = 0; i < total; ++i)
		if (!rafts[i])
			StartServer(i);

	ASSERT_NE(One(iter, total), ~0u);
}
*/
int main(int argc, char **argv)
{
	Signal::Register(SIGINT, []() { FreeRaftSet(); });
	int r = RUN_ALL_TESTS(argc == 2 ? argv[1] : '\0');
	FreeRaftSet();
	return r;
}
