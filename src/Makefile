CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread
DFLAGS =

MushroomFile = $(shell find mushroom -name '*.cpp')
NetworkFile  = $(shell find network -name '*.cpp')

MOBJ = $(MushroomFile:.cpp=.o)
NOBJ = $(NetworkFile:.cpp=.o)

QUEUE = ../test/queue.cpp

THREAD = ../test/thread.cpp

.cpp.o:
		$(CPP) $(CPPFLAGS) -c $< -o $@ $(DFLAGS) $(LDFLAGS)

blinktree_single_thread: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_with_queue: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_multi_thread: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_single_thread:$(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_with_queue: $(MOBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_multi_thread: $(MOBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

network: $(NOBJ)

tag:
	cd .. && ctags -R -f .tags

clean:
	rm */*.o
	rm blinktree_* lsmtree_*
