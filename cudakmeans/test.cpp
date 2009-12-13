#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "kmeans.h"

namespace {

bool deviceAvailable = false;

bool close(float a, float b, float tolerance = 1e-5f)
{
    return std::fabs(a - b) < tolerance;
}

std::vector<Point> threeCentroids()
{
    std::vector<Point> centroids;
    centroids.push_back(makePoint(-5.0f, -5.0f));
    centroids.push_back(makePoint(5.0f, -4.0f));
    centroids.push_back(makePoint(0.0f, 6.0f));
    return centroids;
}

void testAssignPicksTheNearest()
{
    std::vector<Point> points;
    points.push_back(makePoint(-4.9f, -5.1f));
    points.push_back(makePoint(4.8f, -4.2f));
    points.push_back(makePoint(0.2f, 5.7f));

    std::vector<Point> centroids = threeCentroids();
    std::vector<int> labels(3, -1);

    assignHost(&points[0], 3, &centroids[0], 3, &labels[0]);

    assert(labels[0] == 0);
    assert(labels[1] == 1);
    assert(labels[2] == 2);
}

void testUpdateIsTheMean()
{
    std::vector<Point> points;
    points.push_back(makePoint(0.0f, 0.0f));
    points.push_back(makePoint(2.0f, 4.0f));

    std::vector<int> labels(2, 0);
    std::vector<Point> centroids(1, makePoint(9.0f, 9.0f));

    assert(updateCentroids(&points[0], &labels[0], 2, &centroids[0], 1));
    assert(close(centroids[0].x, 1.0f));
    assert(close(centroids[0].y, 2.0f));

    /* nothing moves the second time round */
    assert(!updateCentroids(&points[0], &labels[0], 2, &centroids[0], 1));
}

void testEmptyClusterIsLeftAlone()
{
    std::vector<Point> points(1, makePoint(1.0f, 1.0f));
    std::vector<int> labels(1, 0);

    std::vector<Point> centroids;
    centroids.push_back(makePoint(1.0f, 1.0f));
    centroids.push_back(makePoint(7.0f, 7.0f));

    updateCentroids(&points[0], &labels[0], 1, &centroids[0], 2);

    assert(close(centroids[1].x, 7.0f));
    assert(close(centroids[1].y, 7.0f));
}

void testFitSeparatesTheBlobs()
{
    const int count = 300;
    std::vector<Point> points(count);
    makeClusters(&points[0], count, 1);

    /* the usual lazy start: the first k points */
    std::vector<Point> centroids;
    for (int i = 0; i < 3; ++i)
        centroids.push_back(points[i]);

    std::vector<int> labels(count, -1);
    int rounds = fitHost(&points[0], count, &centroids[0], 3, &labels[0], 100);

    assert(rounds > 0 && rounds < 100);

    /* makeClusters walks the blobs in turn, so points 0, 3, 6 ... all
       belong together and must end up with the same label */
    for (int i = 3; i < count; ++i)
        assert(labels[i] == labels[i % 3]);

    assert(labels[0] != labels[1]);
    assert(labels[1] != labels[2]);
}

void testDeviceAssignmentMatchesHost()
{
    if (!deviceAvailable) return;

    const int count = 1000;
    std::vector<Point> points(count);
    makeClusters(&points[0], count, 7);

    std::vector<Point> centroids = threeCentroids();
    std::vector<int> fromHost(count, -1), fromDevice(count, -2);

    assignHost(&points[0], count, &centroids[0], 3, &fromHost[0]);
    assert(assignDevice(&points[0], count, &centroids[0], 3, &fromDevice[0]) >= 0.0f);

    for (int i = 0; i < count; ++i)
        assert(fromHost[i] == fromDevice[i]);
}

void testDeviceHandlesARaggedLastBlock()
{
    if (!deviceAvailable) return;

    /* 257 points is one full block plus a single thread */
    const int count = 257;
    std::vector<Point> points(count);
    makeClusters(&points[0], count, 3);

    std::vector<Point> centroids = threeCentroids();
    std::vector<int> fromHost(count, -1), fromDevice(count, -2);

    assignHost(&points[0], count, &centroids[0], 3, &fromHost[0]);
    assert(assignDevice(&points[0], count, &centroids[0], 3, &fromDevice[0]) >= 0.0f);

    for (int i = 0; i < count; ++i)
        assert(fromHost[i] == fromDevice[i]);
}

void testBothFitsEndUpInTheSamePlace()
{
    if (!deviceAvailable) return;

    const int count = 600;
    std::vector<Point> points(count);
    makeClusters(&points[0], count, 11);

    std::vector<Point> hostCentroids, deviceCentroids;
    for (int i = 0; i < 3; ++i)
    {
        hostCentroids.push_back(points[i]);
        deviceCentroids.push_back(points[i]);
    }

    std::vector<int> hostLabels(count, -1), deviceLabels(count, -2);

    int hostRounds = fitHost(&points[0], count, &hostCentroids[0], 3, &hostLabels[0], 100);
    int deviceRounds = fitDevice(&points[0], count, &deviceCentroids[0], 3, &deviceLabels[0], 100);

    assert(hostRounds == deviceRounds);
    for (int c = 0; c < 3; ++c)
    {
        assert(close(hostCentroids[c].x, deviceCentroids[c].x, 1e-4f));
        assert(close(hostCentroids[c].y, deviceCentroids[c].y, 1e-4f));
    }
}

void testNoPoints()
{
    Point dummy = makePoint(0.0f, 0.0f);
    int label = 0;
    assert(assignDevice(&dummy, 0, &dummy, 1, &label) < 0.0f);
}

}

int main()
{
    Point probe = makePoint(0.0f, 0.0f);
    int probeLabel = 0;
    deviceAvailable = assignDevice(&probe, 1, &probe, 1, &probeLabel) >= 0.0f;

    testAssignPicksTheNearest();
    testUpdateIsTheMean();
    testEmptyClusterIsLeftAlone();
    testFitSeparatesTheBlobs();
    testNoPoints();
    testDeviceAssignmentMatchesHost();
    testDeviceHandlesARaggedLastBlock();
    testBothFitsEndUpInTheSamePlace();

    if (!deviceAvailable)
    {
        printf("no cuda device here, only the host tests ran\n");
        printf("all tests passed\n");
        return 0;
    }

    const int count = 200000;
    std::vector<Point> points(count);
    makeClusters(&points[0], count, 2009);

    std::vector<Point> centroids = threeCentroids();
    std::vector<int> labels(count, -1);

    float milliseconds = assignDevice(&points[0], count, &centroids[0], 3, &labels[0]);
    printf("%d points against 3 centroids: %.3f ms on the card\n", count, milliseconds);
    printf("all tests passed\n");
    return 0;
}
