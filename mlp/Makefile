CXX      = g++
CXXFLAGS = -O2 -Wall

mlp_test: test.o network.o
	$(CXX) $(CXXFLAGS) -o mlp_test test.o network.o

test.o: test.cpp network.h random.h
network.o: network.cpp network.h random.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o mlp_test
