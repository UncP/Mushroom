CPP  = g++
CPPFLAGS = -std=c++11 -Wall -Wextra -O3 -pthread

File = slice latch latch_manager page page_pool pool_manager b_link_tree task queue \
	thread_pool db

OBJ = slice.o latch.o latch_manager.o page.o page_pool.o pool_manager.o b_link_tree.o task.o \
	queue.o thread_pool.o db.o

btree_test_single_thread:
	for f in $(File); do \
		echo $(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH && \
		$(CPP) $(CPPFLAGS) -c $$f.cpp -o $$f.o -DNOLATCH || exit ; \
	done
	$(CPP) $(CPPFLAGS) -o btree_test_single_thread $(OBJ) ../test/btree_test_with_queue.cpp \
	-DNOLATCH

btree_test_with_queue: $(OBJ) ../test/btree_test_with_queue.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_with_queue $(OBJ) ../test/btree_test_with_queue.cpp

btree_test_multi_thread: $(OBJ) ../test/btree_test_with_queue.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_multi_thread $(OBJ) ../test/btree_test_multi_thread.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o btree_test_* ../.tags