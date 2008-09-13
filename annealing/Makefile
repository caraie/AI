CXX      = g++
CXXFLAGS = -O2 -Wall

annealing_test: test.o tsp.o
	$(CXX) $(CXXFLAGS) -o annealing_test test.o tsp.o

test.o: test.cpp tsp.h random.h
tsp.o: tsp.cpp tsp.h random.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o annealing_test
