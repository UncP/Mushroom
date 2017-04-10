CPP  = g++
CPPFLAGS = -std=c++11 -Wall -Wextra -O3 -pthread

File = slice latch latch_manager page page_pool pool_manager b_link_tree block sstable lsm_tree \
	task queue thread_pool db

OBJ = slice.o latch.o latch_manager.o page.o page_pool.o pool_manager.o b_link_tree.o \
	block.o sstable.o lsm_tree.o task.o queue.o thread_pool.o db.o

blinktree_single_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o blinktree_single_thread $(OBJ) ../test/queue.cpp -DNOLATCH

blinktree_with_queue: $(OBJ) ../test/queue.cpp
	$(CPP) $(CPPFLAGS) -o blinktree_with_queue $(OBJ) ../test/queue.cpp

blinktree_multi_thread: $(OBJ) ../test/multi_thread.cpp
	$(CPP) $(CPPFLAGS) -o blinktree_multi_thread $(OBJ) ../test/multi_thread.cpp

lsmtree_single_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH -DLSM && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH -DLSM || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o lsmtree_single_thread $(OBJ) ../test/queue.cpp -DNOLATCH -DLSM

lsmtree_with_queue:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH -DLSM && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DLSM || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o lsmtree_with_queue $(OBJ) ../test/queue.cpp -DLSM

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o blinktree_* lsmtree_*
