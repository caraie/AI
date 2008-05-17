CXX      = g++
CXXFLAGS = -O2 -Wall

kmeans_test: test.o kmeans.o
	$(CXX) $(CXXFLAGS) -o kmeans_test test.o kmeans.o

test.o: test.cpp kmeans.h
kmeans.o: kmeans.cpp kmeans.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o kmeans_test
