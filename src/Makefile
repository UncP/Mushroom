CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread

MushroomFile = $(shell find mushroom -name '*.cpp')
NetworkFile = $(shell find network -name '*.cpp')
LogFile  = $(shell find log -name '*.cpp')
RaftFile = $(shell find raft -name '*.cpp')
RpcFile = $(shell find rpc -name '*.cpp')

MOBJ = $(MushroomFile:.cpp=.o)
NOBJ = $(NetworkFile:.cpp=.o)
LOBJ = $(LogFile:.cpp=.o)
RaftOBJ = $(RaftFile:.cpp=.o)
RpcOBJ = $(RpcFile:.cpp=.o)

all: blinktree_with_queue_test blinktree_multi_thread_test distributed_index_test

mushroom_log_vector_test: raft/mushroom_log_vector.cpp ../test/mushroom_log_vector.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

distributed_index_test:$(MOBJ) $(LOBJ) $(NOBJ) $(RpcOBJ) $(RaftOBJ) ../test/distributed_index.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

raft_test: $(LOBJ) $(NOBJ) $(RpcOBJ) $(RaftOBJ) ../test/raft.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

rpc_client_test: $(LOBJ) $(NOBJ) $(RpcOBJ) ../test/rpc_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

rpc_server_test: $(LOBJ) $(NOBJ) $(RpcOBJ) ../test/rpc_server.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

client_test: $(LOBJ) $(NOBJ) ../test/network_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

server_test: $(LOBJ) $(NOBJ) ../test/network_server.cpp
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
