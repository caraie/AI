#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "kmeans.h"

namespace {

bool close(double a, double b, double tolerance = 1e-9)
{
    return std::fabs(a - b) < tolerance;
}

/* Two tight groups, one around the origin and one around (10, 10). */
std::vector<Point> twoClusters()
{
    std::vector<Point> points;
    points.push_back(Point(0.0, 0.0));
    points.push_back(Point(10.0, 10.0));
    points.push_back(Point(0.5, 0.2));
    points.push_back(Point(9.6, 10.4));
    points.push_back(Point(0.1, 0.9));
    points.push_back(Point(10.3, 9.7));
    return points;
}

void testDistance()
{
    assert(close(distance(Point(0, 0), Point(3, 4)), 5.0));
    assert(close(distance(Point(2, 2), Point(2, 2)), 0.0));
}

void testSeparatesTwoClusters()
{
    std::vector<Point> points = twoClusters();

    KMeans model(2);
    int rounds = model.fit(points);
    assert(rounds > 0);

    const std::vector<int> &labels = model.labels();

    /* Points 0, 2 and 4 are one group, the rest the other. The labels
       themselves are arbitrary, only the grouping matters. */
    assert(labels[0] == labels[2]);
    assert(labels[0] == labels[4]);
    assert(labels[1] == labels[3]);
    assert(labels[1] == labels[5]);
    assert(labels[0] != labels[1]);
}

void testCentroidsAreTheMeans()
{
    std::vector<Point> points = twoClusters();

    KMeans model(2);
    model.fit(points);

    for (std::size_t c = 0; c < model.centroids().size(); ++c)
    {
        double sumX = 0.0, sumY = 0.0;
        int count = 0;

        for (std::size_t i = 0; i < points.size(); ++i)
        {
            if (model.labels()[i] != (int)c) continue;
            sumX += points[i].x;
            sumY += points[i].y;
            ++count;
        }

        assert(count > 0);
        assert(close(model.centroids()[c].x, sumX / count, 1e-9));
        assert(close(model.centroids()[c].y, sumY / count, 1e-9));
    }
}

void testMoreClustersMeansLessInertia()
{
    std::vector<Point> points = twoClusters();

    KMeans two(2);
    two.fit(points);

    KMeans three(3);
    three.fit(points);

    assert(three.inertia(points) <= two.inertia(points));
}

void testNotEnoughPoints()
{
    std::vector<Point> points;
    points.push_back(Point(1.0, 1.0));

    KMeans model(3);
    assert(model.fit(points) == 0);
}

void testEveryPointGetsALabel()
{
    std::vector<Point> points = twoClusters();

    KMeans model(2);
    model.fit(points);

    assert(model.labels().size() == points.size());
    for (std::size_t i = 0; i < points.size(); ++i)
        assert(model.labels()[i] >= 0 && model.labels()[i] < 2);
}

}

int main()
{
    testDistance();
    testSeparatesTwoClusters();
    testCentroidsAreTheMeans();
    testMoreClustersMeansLessInertia();
    testNotEnoughPoints();
    testEveryPointGetsALabel();

    std::vector<Point> points = twoClusters();
    KMeans model(2);
    int rounds = model.fit(points);
    printf("settled after %d rounds, inertia %.4f\n", rounds, model.inertia(points));
    for (std::size_t c = 0; c < model.centroids().size(); ++c)
        printf("centroid %d at (%.2f, %.2f)\n", (int)c,
               model.centroids()[c].x, model.centroids()[c].y);
    printf("all tests passed\n");
    return 0;
}
