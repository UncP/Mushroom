CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3

OBJ = btree.o

Mushroom:$(OBJ) Mushroom.cpp
	$(CPP) $(CPPFLAGS) -o Mushroom $(OBJ) Mushroom.cpp

btree_page_test:btree.o ../test/btree_page_test.cpp
	$(CPP) $(CPPFLAGS) -o btree_page_test btree.o ../test/btree_page_test.cpp

clean:
	rm *.o Mushroom btree_page_test