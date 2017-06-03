/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-03 09:37:22
**/

#include <vector>

#include "../src/mushroom/db.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/raft/raft_server.hpp"

using namespace std;
using namespace Mushroom;

static EventBase *base = 0;
static vector<RaftServer *> rafts;
static uint16_t port_base = 7000;
static vector<MushroomDB *> db;

static void MakeRaftConfig(int total) {
	FreeRaftSet();
	base = new EventBase(4, 64);
	rafts.resize(total);
	for (int i = 0; i < total; ++i)
		rafts[i] = new RaftServer(base, port_base++, i);
	for (int i = 0; i < total; ++i) {
		for (int j = 0; j < total; ++j) {
			if (i == j) continue;
			rafts[i]->AddPeer(new RpcConnection(EndPoint(rafts[j]->Port(), "127.0.0.1"),
				base->GetPoller(), 0));
		}
	}
}

static void FreeRaftSet() {
	for (auto e : rafts)
		e->Close();
	base->Exit();
	for (auto e : rafts)
		delete e;
	delete base;
}

int main(int argc, char **argv)
{
	const char *file = "../data/10000000";

	assert(argc > 5);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint32_t hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 1024;
	uint32_t seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;
	uint32_t log_page  = atoi(argv[5]) ? atoi(argv[5]) : 16;

	int total = (argc == 7) ? atoi(argv[6]) : 1;

	int all = 3;
	MakeRaftSet(all);
	Signal::Register(SIGINT, [base]() { base->Exit(); });

	for (int i = 0; i < all; i++) {
		db.push_back(new MushroomDB("mushroom_test", key_len, page_size, pool_size, hash_bits,
			seg_bits, log_page));
	}
	for (auto e : rafts)
		e->Start();
	base->Loop();
	FreeRaftSet();
	return 0;
}
