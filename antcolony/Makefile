CXX      = g++
CXXFLAGS = -O2 -Wall

antcolony_test: test.o aco.o
	$(CXX) $(CXXFLAGS) -o antcolony_test test.o aco.o

test.o: test.cpp aco.h random.h
aco.o: aco.cpp aco.h random.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o antcolony_test
