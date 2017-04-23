CPP = $(CXX)
CPPFLAGS = -std=c++11 -Wall -Wextra -O3
LDFLAGS = -pthread
DFLAGS =

File = $(shell find -maxdepth 1 -name '*.cpp')

OBJ = $(File:.cpp=.o)

QUEUE = ../test/queue.cpp

THREAD = ../test/thread.cpp

.cpp.o:
		$(CPP) $(CPPFLAGS) -c $< -o $@ $(DFLAGS) $(LDFLAGS)

blinktree_single_thread: $(OBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_with_queue: $(OBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

blinktree_multi_thread: $(OBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_single_thread:$(OBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_with_queue: $(OBJ) $(QUEUE)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

lsmtree_multi_thread: $(OBJ) $(THREAD)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(DFLAGS) $(LDFLAGS)

tag:
	cd .. && ctags -R -f .tags

clean:
	rm *.o blinktree_* lsmtree_*
