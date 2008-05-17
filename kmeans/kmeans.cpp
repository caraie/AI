#include "kmeans.h"

#include <cmath>
#include <cstddef>

double distance(const Point &a, const Point &b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

KMeans::KMeans(int clusters, int maxIterations)
    : m_clusters(clusters), m_maxIterations(maxIterations)
{
}

int KMeans::nearest(const Point &point) const
{
    int best = 0;
    double bestDistance = distance(point, m_centroids[0]);

    for (std::size_t i = 1; i < m_centroids.size(); ++i)
    {
        double d = distance(point, m_centroids[i]);
        if (d < bestDistance)
        {
            bestDistance = d;
            best = (int)i;
        }
    }

    return best;
}

/* Returns true when at least one point changed cluster. */
bool KMeans::assign(const std::vector<Point> &points)
{
    bool moved = false;

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        int cluster = nearest(points[i]);
        if (m_labels[i] != cluster)
        {
            m_labels[i] = cluster;
            moved = true;
        }
    }

    return moved;
}

void KMeans::update(const std::vector<Point> &points)
{
    std::vector<Point> sums(m_clusters);
    std::vector<int> counts(m_clusters, 0);

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        int cluster = m_labels[i];
        sums[cluster].x += points[i].x;
        sums[cluster].y += points[i].y;
        counts[cluster] += 1;
    }

    for (int c = 0; c < m_clusters; ++c)
    {
        /* An empty cluster has no mean to move to, so leave the centroid
           where it is and let the next round decide. */
        if (counts[c] == 0) continue;

        m_centroids[c].x = sums[c].x / counts[c];
        m_centroids[c].y = sums[c].y / counts[c];
    }
}

int KMeans::fit(const std::vector<Point> &points)
{
    if ((int)points.size() < m_clusters) return 0;

    m_centroids.assign(points.begin(), points.begin() + m_clusters);
    m_labels.assign(points.size(), -1);

    for (int iteration = 1; iteration <= m_maxIterations; ++iteration)
    {
        if (!assign(points)) return iteration;
        update(points);
    }

    return m_maxIterations;
}

double KMeans::inertia(const std::vector<Point> &points) const
{
    double total = 0.0;

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        double d = distance(points[i], m_centroids[m_labels[i]]);
        total += d * d;
    }

    return total;
}
