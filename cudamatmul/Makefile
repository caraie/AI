CUDA_HOME = /usr/local/cuda

CXX       = g++
CXXFLAGS  = -O2 -Wall
NVCC      = $(CUDA_HOME)/bin/nvcc
NVCCFLAGS = -O2 -arch sm_11

LDFLAGS   = -L$(CUDA_HOME)/lib -lcudart

cudamatmul_test: test.o matmul_ref.o matmul.o
	$(CXX) -o cudamatmul_test test.o matmul_ref.o matmul.o $(LDFLAGS)

test.o: test.cpp matmul.h
	$(CXX) $(CXXFLAGS) -c test.cpp

matmul_ref.o: matmul_ref.cpp matmul.h
	$(CXX) $(CXXFLAGS) -c matmul_ref.cpp

matmul.o: matmul.cu matmul.h
	$(NVCC) $(NVCCFLAGS) -I$(CUDA_HOME)/include -c matmul.cu

# nvcc -deviceemu builds the kernels for the cpu, which is the only way
# to run this on a machine without a card. It is slow as treacle but it
# does catch the indexing mistakes.
emu: NVCCFLAGS = -O2 -deviceemu
emu: clean cudamatmul_test

clean:
	rm -f *.o cudamatmul_test
