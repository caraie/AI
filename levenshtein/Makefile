CXX      = g++
CXXFLAGS = -O2 -Wall

levenshtein_test: test.o editdistance.o
	$(CXX) $(CXXFLAGS) -o levenshtein_test test.o editdistance.o

test.o: test.cpp editdistance.h
editdistance.o: editdistance.cpp editdistance.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o levenshtein_test
