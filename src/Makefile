CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch_manager.o btree_pager.o data_page.o btree.o db.o iterator.o queue.o \
      thread_pool.o

Mushroom:$(OBJ) Mushroom.cpp
	$(CPP) $(CPPFLAGS) -o Mushroom $(OBJ) Mushroom.cpp

btree_page_test:btree_pager.o ../test/btree_page_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_page_test btree_pager.o ../test/btree_page_test.cpp

btree_pager_test:btree_pager.o ../test/btree_pager_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_pager_test btree_pager.o ../test/btree_pager_test.cpp

slice_test:slice.o ../test/slice_test.cpp
	$(CPP) $(CPPFLAGS) -o slice_test slice.o ../test/slice_test.cpp

btree_test:btree_pager.o btree.o ../test/btree_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_test btree_pager.o btree.o ../test/btree_test.cpp

db_test:$(OBJ) ../test/db_test.cpp
	$(CPP) $(CPPFLAGS) -o db_test $(OBJ) ../test/db_test.cpp

thread_pool_test:$(OBJ) ../test/thread_pool_test.cpp
	$(CPP) $(CPPFLAGS) -o thread_pool_test $(OBJ) ../test/thread_pool_test.cpp

queue_test:$(OBJ) ../test/queue_test.cpp
	$(CPP) $(CPPFLAGS) -o queue_test $(OBJ) ../test/queue_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o Mushroom btree_page_test slice_test btree_test db_test thread_pool_test queue_test \
	../.tags