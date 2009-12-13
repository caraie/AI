#ifndef KMEANS_H
#define KMEANS_H

/*
 * k means where the assignment step runs on the graphics card.
 *
 * That step is the one worth moving: every point has to be measured
 * against every centroid and none of those measurements depend on each
 * other. The update step is a handful of sums over k clusters and it
 * stays on the host, where writing it takes five minutes instead of an
 * afternoon of atomics.
 *
 * Point is left as a plain struct with no constructors on purpose, it has
 * to go into __shared__ memory.
 */
struct Point
{
    float x;
    float y;
};

Point makePoint(float x, float y);

/* Distances are compared squared, the square root is monotonic and costs
   a special function unit slot per point. */
void assignHost(const Point *points, int count,
                const Point *centroids, int clusters, int *labels);

/* Same thing on the card. Returns the kernel time in milliseconds, or -1
   when there is no device. */
float assignDevice(const Point *points, int count,
                   const Point *centroids, int clusters, int *labels);

/* Moves every centroid to the mean of its points. Returns true if any of
   them actually moved. */
bool updateCentroids(const Point *points, const int *labels, int count,
                     Point *centroids, int clusters);

/* Lloyd's loop with the centroids given by the caller. Returns the number
   of iterations used, or -1 from fitDevice when there is no card. */
int fitHost(const Point *points, int count, Point *centroids, int clusters,
            int *labels, int maxIterations);
int fitDevice(const Point *points, int count, Point *centroids, int clusters,
              int *labels, int maxIterations);

/* Three blobs with a bit of scatter, same numbers on every machine. */
void makeClusters(Point *points, int count, unsigned long seed);

#endif
