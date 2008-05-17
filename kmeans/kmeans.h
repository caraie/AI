#ifndef KMEANS_H
#define KMEANS_H

#include <vector>

struct Point
{
    double x;
    double y;

    Point() : x(0.0), y(0.0) {}
    Point(double px, double py) : x(px), y(py) {}
};

double distance(const Point &a, const Point &b);

/*
 * Lloyd's algorithm. The starting centroids are the first k points of the
 * set: not the best choice, but it keeps the result repeatable, which for
 * a test is worth more than a slightly better split.
 */
class KMeans
{
public:
    KMeans(int clusters, int maxIterations = 100);

    /* Returns the number of iterations it took to settle. */
    int fit(const std::vector<Point> &points);

    const std::vector<Point> &centroids() const { return m_centroids; }
    const std::vector<int> &labels() const { return m_labels; }

    /* Sum of the squared distances of every point to its centroid. */
    double inertia(const std::vector<Point> &points) const;

private:
    bool assign(const std::vector<Point> &points);
    void update(const std::vector<Point> &points);
    int nearest(const Point &point) const;

    int m_clusters;
    int m_maxIterations;
    std::vector<Point> m_centroids;
    std::vector<int> m_labels;
};

#endif
