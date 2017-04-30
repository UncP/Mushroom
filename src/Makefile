CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread
DFLAGS =

MushroomFile = $(shell find mushroom -name '*.cpp')
NetworkFile  = $(shell find network -name '*.cpp')
LogFile  = $(shell find log -name '*.cpp')
RaftFile = $(shell find raft -name '*.cpp')
RpcFile = $(shell find rpc -name '*.cpp')

MOBJ = $(MushroomFile:.cpp=.o)
NOBJ = $(NetworkFile:.cpp=.o)
LOBJ = $(LogFile:.cpp=.o)
RaftOBJ = $(RaftFile:.cpp=.o)
RpcOBJ = $(RpcFile:.cpp=.o)

QUEUE = ../test/mushroom_with_queue.cpp

THREAD = ../test/mushroom_multi_thread.cpp

network: client_test server_test

raft_test: $(RaftOBJ)

rpc_test: $(RpcOBJ) ../test/rpc.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

client_test: $(LOBJ) $(NOBJ) ../test/network_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

server_test: $(LOBJ) $(NOBJ) ../test/network_server.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

blinktree_single_thread_test: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_with_queue_test: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_multi_thread_test: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_single_thread_test: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_with_queue_test: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_multi_thread_test: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

.cpp.o:
		$(CPP) $(CPPFLAGS) -c $< -o $@ $(DFLAGS) $(LDFLAGS)

tag:
	cd .. && ctags -R -f .tags

clean:
	rm */*.o
	rm *_test
