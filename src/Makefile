CPP 		 = g++
CPPFLAGS = -std=c++14 -Wall -O3 -pthread
DFLAGS   =

OBJ = slice latch latch_manager btree_page btree task queue iterator thread_pool db

db_test:../test/db_test.cpp
	for e in $(OBJ); do \
		$(CPP) $(CPPFLAGS) -c $$e.cpp -o $$e.o $(DFLAGS) || exit; \
	done
	$(CPP) $(CPPFLAGS) -o db_test *.o $(DFLAGS) ../test/db_test.cpp

tag:
	cd .. && ctags -R -f .tags -h .cpp -h .hpp

clean:
	rm *.o *_test ../.tags