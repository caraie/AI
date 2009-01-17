CXX      = g++
CXXFLAGS = -O2 -Wall

linreg_test: test.o regression.o
	$(CXX) $(CXXFLAGS) -o linreg_test test.o regression.o

test.o: test.cpp regression.h
regression.o: regression.cpp regression.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o linreg_test
