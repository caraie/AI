CXX      = g++
CXXFLAGS = -O2 -Wall

hopfield_test: test.o hopfield.o
	$(CXX) $(CXXFLAGS) -o hopfield_test test.o hopfield.o

test.o: test.cpp hopfield.h
hopfield.o: hopfield.cpp hopfield.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o hopfield_test
