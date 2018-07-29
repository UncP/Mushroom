CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread

BlinkFile = $(shell find blink -name '*.cpp')
NetworkFile = $(shell find network -name '*.cpp')
RaftFile = $(shell find raft -name '*.cpp')
RpcFile = $(shell find rpc -name '*.cpp')
ARTFILE = $(shell find art -name '*.cpp')
PalmFile = $(shell find palm -name '*.cpp')

MOBJ = $(BlinkFile:.cpp=.o)
NOBJ = $(NetworkFile:.cpp=.o)
RaftOBJ = $(RaftFile:.cpp=.o)
RpcOBJ = $(RpcFile:.cpp=.o)
AOBJ = $(ARTFILE:.cpp=.o)
POBJ = $(PalmFile:.cpp=.o)

default: palm_tree_test

all: blinktree_with_queue_test blinktree_multi_thread_test palm_tree_test

palm_tree_test: $(POBJ) ./blink/pool_manager.o ./blink/page_pool.o ./blink/page.o ../test/palm.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

batcher_test: ../test/batcher.cpp ./palm/batcher.o ./blink/page.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

batch_test: ../test/batch.cpp ./palm/batch.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

barrier_test: ../test/barrier.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

art_tree_test: $(AOBJ) ../test/art.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ -msse2

mushroom_log_vector_test: raft/mushroom_log_vector.cpp ../test/mushroom_log_vector.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

distributed_index_test: $(MOBJ) $(NOBJ) $(RpcOBJ) $(RaftOBJ) ../test/distributed_index.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

raft_test:  $(NOBJ) $(RpcOBJ) $(RaftOBJ) ../test/raft.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

rpc_client_test: $(NOBJ) $(RpcOBJ) ../test/rpc_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

rpc_server_test: $(NOBJ) $(RpcOBJ) ../test/rpc_server.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

client_test: $(NOBJ) ../test/network_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

server_test: $(NOBJ) ../test/network_server.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

page_test: $(MOBJ) ../test/page.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

blinktree_with_queue_test: $(MOBJ) ../test/mushroom_with_queue.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

blinktree_multi_thread_test: $(MOBJ) ../test/mushroom_multi_thread.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

.cpp.o:
		$(CPP) $(CPPFLAGS) -c $< -o $@ $(DFLAGS) $(LDFLAGS)

tag:
	cd .. && ctags -R -f .tags

clean:
	rm */*.o
	rm -rf *_test
