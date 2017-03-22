CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch.o latch_manager.o btree_page.o page_manager.o btree.o task.o queue.o \
	iterator.o thread_pool.o db.o

btree_test_with_queue:$(OBJ) ../test/btree_test_with_queue.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_with_queue $(OBJ) ../test/btree_test_with_queue.cpp

btree_test_multi_thread:$(OBJ) ../test/btree_test_multi_thread.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_multi_thread $(OBJ) ../test/btree_test_multi_thread.cpp

btree_test_multi_process:$(OBJ) ../test/btree_test_multi_process.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_multi_process $(OBJ) ../test/btree_test_multi_process.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o btree_test_* ../.tags