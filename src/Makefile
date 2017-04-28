CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread
DFLAGS =

MushroomFile = $(shell find mushroom -name '*.cpp')
NetworkFile  = $(shell find network -name '*.cpp')

MOBJ = $(MushroomFile:.cpp=.o)
NOBJ = $(NetworkFile:.cpp=.o)

QUEUE = ../test/mushroom_with_queue.cpp

THREAD = ../test/mushroom_multi_thread.cpp

network: client server

client: $(NOBJ) ../test/network_client.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

server: $(NOBJ) ../test/network_server.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

blinktree_single_thread: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_with_queue: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_multi_thread: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_single_thread: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_with_queue: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_multi_thread: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

.cpp.o:
		$(CPP) $(CPPFLAGS) -c $< -o $@ $(DFLAGS) $(LDFLAGS)

tag:
	cd .. && ctags -R -f .tags

clean:
	rm */*.o
	rm blinktree_* lsmtree_* client server
