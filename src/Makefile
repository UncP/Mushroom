CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread

OBJ = slice.o latch.o latch_manager.o btree_page.o btree.o task.o queue.o iterator.o \
	thread_pool.o db.o

btree_test_single:$(OBJ) ../test/btree_test_single.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_single $(OBJ) ../test/btree_test_single.cpp

btree_test_multiple:$(OBJ) ../test/btree_test_multiple.cpp
	$(CPP) $(CPPFLAGS) -o btree_test_multiple $(OBJ) ../test/btree_test_multiple.cpp

lm_test:$(OBJ) ../test/latch_manager_test.cpp
	$(CPP) $(CPPFLAGS) -o lm_test $(OBJ) ../test/latch_manager_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o btree_test_single btree_test_multiple ../.tags