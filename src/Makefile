CPP  = g++
CPPFLAGS = -std=c++11 -Wall -Wextra -O3 -pthread

File = slice latch latch_manager page page_pool pool_manager b_link_tree block block_manager \
	sstable sstable_manager level level_tree lsm_tree task queue thread_pool db

OBJ = slice.o latch.o latch_manager.o page.o page_pool.o pool_manager.o b_link_tree.o \
	block.o block_manager.o sstable.o sstable_manager.o level.o level_tree.o lsm_tree.o \
	task.o queue.o thread_pool.o db.o

blinktree_single_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH -DNOLSM && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH -DNOLSM || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o blinktree_single_thread $(OBJ) ../test/queue.cpp -DNOLATCH -DNOLSM

blinktree_with_queue:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLSM && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLSM || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o blinktree_with_queue $(OBJ) ../test/queue.cpp -DNOLSM

blinktree_multi_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLSM && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLSM || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o blinktree_multi_thread $(OBJ) ../test/multi_thread.cpp -DNOLSM

lsmtree_single_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o lsmtree_single_thread $(OBJ) ../test/queue.cpp -DNOLATCH

lsmtree_with_queue: $(OBJ) ../test/queue.cpp
	$(CPP) $(CPPFLAGS) -o lsmtree_with_queue $(OBJ) ../test/queue.cpp

lsmtree_multi_thread: $(OBJ) ../test/multi_thread.cpp
	$(CPP) $(CPPFLAGS) -o lsmtree_multi_thread $(OBJ) ../test/multi_thread.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o blinktree_* lsmtree_*
