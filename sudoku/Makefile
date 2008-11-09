CXX      = g++
CXXFLAGS = -O2 -Wall

sudoku_test: test.o sudoku.o
	$(CXX) $(CXXFLAGS) -o sudoku_test test.o sudoku.o

test.o: test.cpp sudoku.h
sudoku.o: sudoku.cpp sudoku.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o sudoku_test
