CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch.o latch_manager.o btree_page.o btree_pager.o \
			data_page.o data_pager.o btree.o db.o iterator.o queue.o thread_pool.o

slice_test:$(OBJ) ../test/slice_test.cpp
	$(CPP) $(CPPFLAGS) -o slice_test $(OBJ) ../test/slice_test.cpp

btree_test:$(OBJ) ../test/btree_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_test $(OBJ) ../test/btree_test.cpp

key_test:$(OBJ) ../test/key_test.cpp
	$(CPP) $(CPPFLAGS) -o key_test $(OBJ) ../test/key_test.cpp

thread_pool_test:$(OBJ) ../test/thread_pool_test.cpp
	$(CPP) $(CPPFLAGS) -o thread_pool_test $(OBJ) ../test/thread_pool_test.cpp

db_test:$(OBJ) ../test/db_test.cpp
	$(CPP) $(CPPFLAGS) -o db_test $(OBJ) ../test/db_test.cpp

multi_thread_btree_test:$(OBJ) ../test/multi_thread_btree_test.cpp
	$(CPP) $(CPPFLAGS) -o multi_thread_btree_test $(OBJ) ../test/multi_thread_btree_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o slice_test btree_test key_test thread_pool_test db_test  multi_thread_btree_test \
	../.tags