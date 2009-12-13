#include "kmeans.h"

#include <cmath>

Point makePoint(float x, float y)
{
    Point point;
    point.x = x;
    point.y = y;
    return point;
}

void assignHost(const Point *points, int count,
                const Point *centroids, int clusters, int *labels)
{
    for (int i = 0; i < count; ++i)
    {
        int best = 0;
        float bestDistance = -1.0f;

        for (int c = 0; c < clusters; ++c)
        {
            float dx = points[i].x - centroids[c].x;
            float dy = points[i].y - centroids[c].y;
            float distance = dx * dx + dy * dy;

            if (bestDistance < 0.0f || distance < bestDistance)
            {
                bestDistance = distance;
                best = c;
            }
        }

        labels[i] = best;
    }
}

bool updateCentroids(const Point *points, const int *labels, int count,
                     Point *centroids, int clusters)
{
    bool moved = false;

    for (int c = 0; c < clusters; ++c)
    {
        float sumX = 0.0f;
        float sumY = 0.0f;
        int members = 0;

        for (int i = 0; i < count; ++i)
        {
            if (labels[i] != c) continue;
            sumX += points[i].x;
            sumY += points[i].y;
            ++members;
        }

        /* an empty cluster has no mean, leave it where it is */
        if (members == 0) continue;

        float x = sumX / members;
        float y = sumY / members;

        if (x != centroids[c].x || y != centroids[c].y) moved = true;

        centroids[c].x = x;
        centroids[c].y = y;
    }

    return moved;
}

namespace {

int fit(const Point *points, int count, Point *centroids, int clusters,
        int *labels, int maxIterations, bool onDevice)
{
    for (int iteration = 1; iteration <= maxIterations; ++iteration)
    {
        if (onDevice)
        {
            if (assignDevice(points, count, centroids, clusters, labels) < 0.0f)
                return -1;
        }
        else
        {
            assignHost(points, count, centroids, clusters, labels);
        }

        if (!updateCentroids(points, labels, count, centroids, clusters))
            return iteration;
    }

    return maxIterations;
}

}

int fitHost(const Point *points, int count, Point *centroids, int clusters,
            int *labels, int maxIterations)
{
    return fit(points, count, centroids, clusters, labels, maxIterations, false);
}

int fitDevice(const Point *points, int count, Point *centroids, int clusters,
              int *labels, int maxIterations)
{
    return fit(points, count, centroids, clusters, labels, maxIterations, true);
}

void makeClusters(Point *points, int count, unsigned long seed)
{
    const float centresX[3] = {-5.0f, 5.0f, 0.0f};
    const float centresY[3] = {-5.0f, -4.0f, 6.0f};

    unsigned long state = seed;

    for (int i = 0; i < count; ++i)
    {
        int blob = i % 3;

        state = state * 1103515245UL + 12345UL;
        float dx = (float)((state >> 16) & 0x7fffUL) / 32768.0f - 0.5f;
        state = state * 1103515245UL + 12345UL;
        float dy = (float)((state >> 16) & 0x7fffUL) / 32768.0f - 0.5f;

        points[i].x = centresX[blob] + dx * 3.0f;
        points[i].y = centresY[blob] + dy * 3.0f;
    }
}
