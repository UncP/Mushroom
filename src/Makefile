CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch.o latch_manager.o btree_page.o btree_pager.o \
			data_page.o data_pager.o btree.o db.o queue.o thread_pool.o

single_thread_btree_test:$(OBJ) ../test/single_thread_btree_test.cpp
	$(CPP) $(CPPFLAGS) -o single_thread_btree_test $(OBJ) ../test/single_thread_btree_test.cpp

multi_thread_btree_test:$(OBJ) ../test/multi_thread_btree_test.cpp
	$(CPP) $(CPPFLAGS) -o multi_thread_btree_test $(OBJ) ../test/multi_thread_btree_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o slice_test btree_test key_test thread_pool_test single_thread_btree_test \
	multi_thread_btree_test ../.tags