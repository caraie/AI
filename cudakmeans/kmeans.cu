#include <cuda_runtime.h>

#include <cstdio>

#include "kmeans.h"

/* 256 threads per block, one point each. */
#define BLOCK 256

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

/*
 * One thread per point. Every thread in the block reads the same k
 * centroids, so they go into shared memory once instead of being pulled
 * from global memory k times per point.
 *
 * The size of that array is not known at compile time, it comes in as the
 * third argument of the launch.
 */
__global__ void assignKernel(const Point *points, int count,
                             const Point *centroids, int clusters,
                             int *labels)
{
    extern __shared__ Point sharedCentroids[];

    for (int i = threadIdx.x; i < clusters; i += blockDim.x)
        sharedCentroids[i] = centroids[i];

    __syncthreads();

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index >= count) return;

    Point point = points[index];

    int best = 0;
    float bestDistance = -1.0f;

    for (int c = 0; c < clusters; ++c)
    {
        float dx = point.x - sharedCentroids[c].x;
        float dy = point.y - sharedCentroids[c].y;
        float distance = dx * dx + dy * dy;

        if (bestDistance < 0.0f || distance < bestDistance)
        {
            bestDistance = distance;
            best = c;
        }
    }

    labels[index] = best;
}

float assignDevice(const Point *points, int count,
                   const Point *centroids, int clusters, int *labels)
{
    if (count <= 0 || clusters <= 0 || !haveDevice()) return -1.0f;

    const size_t pointBytes = (size_t)count * sizeof(Point);
    const size_t centroidBytes = (size_t)clusters * sizeof(Point);
    const size_t labelBytes = (size_t)count * sizeof(int);

    Point *devicePoints = 0;
    Point *deviceCentroids = 0;
    int *deviceLabels = 0;

    if (failed(cudaMalloc((void **)&devicePoints, pointBytes), "cudaMalloc points"))
        return -1.0f;
    if (failed(cudaMalloc((void **)&deviceCentroids, centroidBytes), "cudaMalloc centroids"))
    {
        cudaFree(devicePoints);
        return -1.0f;
    }
    if (failed(cudaMalloc((void **)&deviceLabels, labelBytes), "cudaMalloc labels"))
    {
        cudaFree(devicePoints);
        cudaFree(deviceCentroids);
        return -1.0f;
    }

    cudaMemcpy(devicePoints, points, pointBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(deviceCentroids, centroids, centroidBytes, cudaMemcpyHostToDevice);

    int blocks = (count + BLOCK - 1) / BLOCK;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    assignKernel<<<blocks, BLOCK, centroidBytes>>>(devicePoints, count,
                                                   deviceCentroids, clusters,
                                                   deviceLabels);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float milliseconds = -1.0f;
    if (!failed(cudaGetLastError(), "kernel"))
    {
        cudaEventElapsedTime(&milliseconds, start, stop);
        cudaMemcpy(labels, deviceLabels, labelBytes, cudaMemcpyDeviceToHost);
    }

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(devicePoints);
    cudaFree(deviceCentroids);
    cudaFree(deviceLabels);

    return milliseconds;
}
