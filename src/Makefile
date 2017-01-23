CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch.o latch_manager.o btree_page.o btree.o task.o queue.o iterator.o \
	thread_pool.o db.o

multi_thread_btree_test:$(OBJ) ../test/multi_thread_btree_test.cpp
	$(CPP) $(CPPFLAGS) -o multi_thread_btree_test $(OBJ) ../test/multi_thread_btree_test.cpp

single_thread_btree_test:$(OBJ) ../test/single_thread_btree_test.cpp
	$(CPP) $(CPPFLAGS) -o single_thread_btree_test $(OBJ) ../test/single_thread_btree_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o single_thread_btree_test multi_thread_btree_test ../.tags