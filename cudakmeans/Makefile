CUDA_HOME = /usr/local/cuda

CXX       = g++
CXXFLAGS  = -O2 -Wall
NVCC      = $(CUDA_HOME)/bin/nvcc
NVCCFLAGS = -O2 -arch sm_11

LDFLAGS   = -L$(CUDA_HOME)/lib -lcudart

cudakmeans_test: test.o kmeans_ref.o kmeans.o
	$(CXX) -o cudakmeans_test test.o kmeans_ref.o kmeans.o $(LDFLAGS)

test.o: test.cpp kmeans.h
	$(CXX) $(CXXFLAGS) -c test.cpp

kmeans_ref.o: kmeans_ref.cpp kmeans.h
	$(CXX) $(CXXFLAGS) -c kmeans_ref.cpp

kmeans.o: kmeans.cu kmeans.h
	$(NVCC) $(NVCCFLAGS) -I$(CUDA_HOME)/include -c kmeans.cu

emu: NVCCFLAGS = -O2 -deviceemu
emu: clean cudakmeans_test

clean:
	rm -f *.o cudakmeans_test
