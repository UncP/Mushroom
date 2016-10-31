CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o mutex.o shared_lock.o latch_manager.o btree_page.o btree_pager.o data_page.o \
			data_pager.o btree.o db.o iterator.o queue.o thread_pool.o

Mushroom:$(OBJ) Mushroom.cpp
	$(CPP) $(CPPFLAGS) -o Mushroom $(OBJ) Mushroom.cpp

btree_page_test:$(OBJ) ../test/btree_page_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_page_test $(OBJ) ../test/btree_page_test.cpp

btree_pager_test:$(OBJ) ../test/btree_pager_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_pager_test $(OBJ) ../test/btree_pager_test.cpp

slice_test:$(OBJ) ../test/slice_test.cpp
	$(CPP) $(CPPFLAGS) -o slice_test $(OBJ) ../test/slice_test.cpp

btree_test:$(OBJ) ../test/btree_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_test $(OBJ) ../test/btree_test.cpp

key_test:$(OBJ) ../test/key_test.cpp
	$(CPP) $(CPPFLAGS) -o key_test $(OBJ) ../test/key_test.cpp

thread_pool_test:$(OBJ) ../test/thread_pool_test.cpp
	$(CPP) $(CPPFLAGS) -o thread_pool_test $(OBJ) ../test/thread_pool_test.cpp

queue_test:$(OBJ) ../test/queue_test.cpp
	$(CPP) $(CPPFLAGS) -o queue_test $(OBJ) ../test/queue_test.cpp

shared_lock_test:$(OBJ) ../test/shared_lock_test.cpp
	$(CPP) $(CPPFLAGS) -o shared_lock_test $(OBJ) ../test/shared_lock_test.cpp

data_page_test:$(OBJ) ../test/data_page_test.cpp
	$(CPP) $(CPPFLAGS) -o data_page_test $(OBJ) ../test/data_page_test.cpp

db_test:$(OBJ) ../test/db_test.cpp
	$(CPP) $(CPPFLAGS) -o db_test $(OBJ) ../test/db_test.cpp

latch_manager_test:shared_lock.o latch_manager.o ../test/latch_manager_test.cpp
	$(CPP) $(CPPFLAGS) -o latch_manager_test shared_lock.o latch_manager.o ../test/latch_manager_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o Mushroom btree_page_test slice_test btree_test key_test thread_pool_test queue_test \
	shared_lock_test data_page_test db_test ../.tags