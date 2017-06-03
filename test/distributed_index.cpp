/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-03 09:37:22
**/

#include <unistd.h>
#include <fcntl.h>
#include <vector>

#include "../src/mushroom/db.hpp"
#include "../src/network/signal.hpp"
#include "../src/rpc/rpc_connection.hpp"
#include "../src/raft/raft_server.hpp"
#include "../src/include/thread.hpp"
#include "../src/raft/mushroom_log.hpp"

using namespace std;
using namespace Mushroom;

static EventBase *base = 0;
static Thread *loop = 0;
static vector<RaftServer *> rafts;
static uint16_t port_base = 7000;

static void MakeRaftConfig(int total) {
	base = new EventBase(4, 64);
	loop = new Thread([&]() { base->Loop(); });
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
	loop->Start();
	for (auto e : rafts)
		e->Start();
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
}

int main(int argc, char **argv)
{
	const char *file = "../data/10000000";

	assert(argc > 5);
	uint32_t page_size = atoi(argv[1]) ? atoi(argv[1]) : 4096;
	uint32_t pool_size = atoi(argv[2]) ? atoi(argv[2]) : 4800;
	uint32_t hash_bits = atoi(argv[3]) ? atoi(argv[3]) : 10;
	uint32_t seg_bits  = atoi(argv[4]) ? atoi(argv[4]) : 4;
	uint32_t log_page  = atoi(argv[5]) ? atoi(argv[5]) : 16;

	int total = (argc == 7) ? atoi(argv[6]) : 1;
	const int key_len = 16;

	int all = 3;
	MakeRaftConfig(all);
	Signal::Register(SIGINT, []() { FreeRaftSet(); });

	vector<MushroomDB *> db;
	for (int i = 0; i < all; i++) {
		db.push_back(new MushroomDB("mushroom_test", key_len, page_size, pool_size, hash_bits,
			seg_bits, log_page));
		MushroomDB *cur = db[i];
		rafts[i]->SetApplyFunc([cur](const MushroomLog &log) {
			return cur->Put(log.key_);
		});
	}

	sleep(1);

	MushroomLog log(key_len);
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int curr = 0, ptr = 0, count = 0;
	bool flag = true;
	for (; (ptr = pread(fd, buf, 8192, curr)) > 0 && flag; curr += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr;) {
			int j = 0;
			char *tmp = buf + i;
			for (; buf[i] != '\n' && buf[i] != '\0'; ++i, ++j) ;
			tmp[j] = '\0';
			memcpy(log.key_->key_, tmp, key_len);
			uint32_t index;
			for (auto e : rafts)
				if (e->Start(log, &index))
					break;
			if (++count == total) {
				flag = false;
				break;
			}
			++i;
		}
	}
	close(fd);

	sleep(2);
	FreeRaftSet();
	flag = true;
	for (uint32_t i = 1; i < db.size(); ++i)
		if (!(*db[0] == *db[i]))
			flag = false;
	for (auto e : db) {
		e->Close();
		delete e;
	}
	printf("\033[31mtotal: %d\033[0m\n", total);
	if (!flag)
		printf("\033[31mfailed :(\033[0m\n");
	else
		printf("\033[32msuccess :)\033[0m\n");
	return 0;
}
