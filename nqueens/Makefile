CXX      = g++
CXXFLAGS = -O2 -Wall

nqueens_test: test.o nqueens.o
	$(CXX) $(CXXFLAGS) -o nqueens_test test.o nqueens.o

test.o: test.cpp nqueens.h
nqueens.o: nqueens.cpp nqueens.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o nqueens_test
