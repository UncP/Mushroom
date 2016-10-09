CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3

OBJ = slice.o bt_page.o btree.o

Mushroom:$(OBJ) Mushroom.cpp
	$(CPP) $(CPPFLAGS) -o Mushroom $(OBJ) Mushroom.cpp

btree_page_test:btree.o ../test/btree_page_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_page_test btree.o ../test/btree_page_test.cpp

slice_test:slice.o ../test/slice_test.cpp
	$(CPP) $(CPPFLAGS) -o slice_test slice.o ../test/slice_test.cpp

btree_test:btree.o ../test/btree_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_test btree.o ../test/btree_test.cpp

clean:
	rm *.o Mushroom btree_page_test slice_test btree_test