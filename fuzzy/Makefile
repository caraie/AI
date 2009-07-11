CXX      = g++
CXXFLAGS = -O2 -Wall

fuzzy_test: test.o fuzzy.o
	$(CXX) $(CXXFLAGS) -o fuzzy_test test.o fuzzy.o

test.o: test.cpp fuzzy.h
fuzzy.o: fuzzy.cpp fuzzy.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o fuzzy_test
