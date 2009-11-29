#include <cuda_runtime.h>

#include <cstdio>

#include "matmul.h"

/* One block computes a 16 x 16 patch of the result. 256 threads is the
   sweet spot on the G8x boards: 512 fits but leaves no registers. */
#define TILE 16

namespace {

bool failed(cudaError_t status, const char *what)
{
    if (status == cudaSuccess) return false;
    fprintf(stderr, "cuda error in %s: %s\n", what, cudaGetErrorString(status));
    return true;
}

bool haveDevice()
{
    int count = 0;
    if (cudaGetDeviceCount(&count) != cudaSuccess) return false;
    return count > 0;
}

}

__global__ void matmulNaiveKernel(const float *a, const float *b, float *c, int n)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int column = blockIdx.x * blockDim.x + threadIdx.x;

    if (row >= n || column >= n) return;

    float sum = 0.0f;
    for (int k = 0; k < n; ++k)
        sum += a[row * n + k] * b[k * n + column];

    c[row * n + column] = sum;
}

/*
 * Tiled version: each block copies a strip of a and a strip of b into
 * shared memory and multiplies them there, so every value read from
 * global memory is used TILE times instead of once.
 *
 * The bounds check has to be done on the load and not with an early
 * return: a thread that leaves before __syncthreads() hangs the rest of
 * the block. That one cost me an evening.
 */
__global__ void matmulTiledKernel(const float *a, const float *b, float *c, int n)
{
    __shared__ float tileA[TILE][TILE];
    __shared__ float tileB[TILE][TILE];

    int row = blockIdx.y * TILE + threadIdx.y;
    int column = blockIdx.x * TILE + threadIdx.x;

    float sum = 0.0f;
    int tiles = (n + TILE - 1) / TILE;

    for (int tile = 0; tile < tiles; ++tile)
    {
        int aColumn = tile * TILE + threadIdx.x;
        int bRow = tile * TILE + threadIdx.y;

        tileA[threadIdx.y][threadIdx.x] =
            (row < n && aColumn < n) ? a[row * n + aColumn] : 0.0f;
        tileB[threadIdx.y][threadIdx.x] =
            (bRow < n && column < n) ? b[bRow * n + column] : 0.0f;

        __syncthreads();

        for (int k = 0; k < TILE; ++k)
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];

        /* without this second barrier the fast threads start loading the
           next tile over values the slow ones are still reading */
        __syncthreads();
    }

    if (row < n && column < n)
        c[row * n + column] = sum;
}

namespace {

float run(const float *a, const float *b, float *c, int n, bool tiled)
{
    if (n <= 0 || !haveDevice()) return -1.0f;

    const size_t bytes = (size_t)n * n * sizeof(float);

    float *deviceA = 0;
    float *deviceB = 0;
    float *deviceC = 0;

    if (failed(cudaMalloc((void **)&deviceA, bytes), "cudaMalloc a")) return -1.0f;
    if (failed(cudaMalloc((void **)&deviceB, bytes), "cudaMalloc b"))
    {
        cudaFree(deviceA);
        return -1.0f;
    }
    if (failed(cudaMalloc((void **)&deviceC, bytes), "cudaMalloc c"))
    {
        cudaFree(deviceA);
        cudaFree(deviceB);
        return -1.0f;
    }

    cudaMemcpy(deviceA, a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(deviceB, b, bytes, cudaMemcpyHostToDevice);

    dim3 threads(TILE, TILE);
    dim3 blocks((n + TILE - 1) / TILE, (n + TILE - 1) / TILE);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    if (tiled)
        matmulTiledKernel<<<blocks, threads>>>(deviceA, deviceB, deviceC, n);
    else
        matmulNaiveKernel<<<blocks, threads>>>(deviceA, deviceB, deviceC, n);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float milliseconds = -1.0f;
    if (!failed(cudaGetLastError(), "kernel"))
    {
        cudaEventElapsedTime(&milliseconds, start, stop);
        cudaMemcpy(c, deviceC, bytes, cudaMemcpyDeviceToHost);
    }

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(deviceC);

    return milliseconds;
}

}

float matmulDevice(const float *a, const float *b, float *c, int n)
{
    return run(a, b, c, n, true);
}

float matmulDeviceNaive(const float *a, const float *b, float *c, int n)
{
    return run(a, b, c, n, false);
}
