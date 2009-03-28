#include "aco.h"

#include <cmath>
#include <cstddef>

namespace {

double distance(const City &a, const City &b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

}

double tourLength(const std::vector<City> &cities, const Tour &tour)
{
    if (tour.size() < 2) return 0.0;

    double total = 0.0;
    for (std::size_t i = 0; i < tour.size(); ++i)
        total += distance(cities[tour[i]], cities[tour[(i + 1) % tour.size()]]);
    return total;
}

std::vector<City> circleOfCities(int count, double radius)
{
    const double pi = 3.14159265358979323846;
    std::vector<City> cities;

    for (int i = 0; i < count; ++i)
    {
        double angle = 2.0 * pi * i / count;
        cities.push_back(City(radius * std::cos(angle), radius * std::sin(angle)));
    }

    return cities;
}

AntColony::AntColony(const std::vector<City> &cities, const AcoOptions &options)
    : m_cities(cities), m_options(options), m_bestLength(-1.0)
{
    const std::size_t n = cities.size();

    /* The distances never change, so pay for the square roots once. */
    m_distances.resize(n);
    m_trails.resize(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        m_distances[i].resize(n, 0.0);
        m_trails[i].resize(n, 1.0);

        for (std::size_t j = 0; j < n; ++j)
            m_distances[i][j] = distance(cities[i], cities[j]);
    }
}

int AntColony::chooseNext(int current, const std::vector<bool> &visited,
                          Random &random) const
{
    std::vector<double> weights(m_cities.size(), 0.0);
    double total = 0.0;

    for (std::size_t city = 0; city < m_cities.size(); ++city)
    {
        if (visited[city]) continue;

        double trail = std::pow(m_trails[current][city], m_options.alpha);
        double closeness = std::pow(1.0 / m_distances[current][city], m_options.beta);
        weights[city] = trail * closeness;
        total += weights[city];
    }

    if (total <= 0.0)
    {
        /* Everything underflowed, fall back to the first city left. */
        for (std::size_t city = 0; city < visited.size(); ++city)
            if (!visited[city]) return (int)city;
        return -1;
    }

    /* roulette wheel over the weights */
    double pick = random.uniform() * total;
    double running = 0.0;

    for (std::size_t city = 0; city < weights.size(); ++city)
    {
        if (visited[city]) continue;
        running += weights[city];
        if (running >= pick) return (int)city;
    }

    for (std::size_t city = 0; city < visited.size(); ++city)
        if (!visited[city]) return (int)city;

    return -1;
}

Tour AntColony::walk(Random &random) const
{
    const int n = (int)m_cities.size();

    std::vector<bool> visited(n, false);
    Tour tour;

    int current = random.below(n);
    tour.push_back(current);
    visited[current] = true;

    while ((int)tour.size() < n)
    {
        int next = chooseNext(current, visited, random);
        if (next < 0) break;

        tour.push_back(next);
        visited[next] = true;
        current = next;
    }

    return tour;
}

void AntColony::evaporate()
{
    for (std::size_t i = 0; i < m_trails.size(); ++i)
        for (std::size_t j = 0; j < m_trails.size(); ++j)
            m_trails[i][j] *= (1.0 - m_options.evaporation);
}

void AntColony::deposit(const Tour &tour, double length)
{
    if (length <= 0.0) return;

    double amount = 1.0 / length;
    for (std::size_t i = 0; i < tour.size(); ++i)
    {
        int from = tour[i];
        int to = tour[(i + 1) % tour.size()];
        m_trails[from][to] += amount;
        m_trails[to][from] += amount;
    }
}

Tour AntColony::run(Random &random)
{
    if (m_cities.size() < 3)
    {
        m_best.clear();
        for (std::size_t i = 0; i < m_cities.size(); ++i)
            m_best.push_back((int)i);
        m_bestLength = tourLength(m_cities, m_best);
        return m_best;
    }

    for (int iteration = 0; iteration < m_options.iterations; ++iteration)
    {
        std::vector<Tour> tours;
        std::vector<double> lengths;

        for (int ant = 0; ant < m_options.ants; ++ant)
        {
            Tour tour = walk(random);
            double length = tourLength(m_cities, tour);

            tours.push_back(tour);
            lengths.push_back(length);

            if (m_bestLength < 0.0 || length < m_bestLength)
            {
                m_best = tour;
                m_bestLength = length;
            }
        }

        evaporate();
        for (std::size_t i = 0; i < tours.size(); ++i)
            deposit(tours[i], lengths[i]);
    }

    return m_best;
}
