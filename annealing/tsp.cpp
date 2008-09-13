#include "tsp.h"

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
    {
        /* the last leg closes the loop back to the first city */
        const City &from = cities[tour[i]];
        const City &to = cities[tour[(i + 1) % tour.size()]];
        total += distance(from, to);
    }

    return total;
}

void reverseSegment(Tour &tour, int i, int j)
{
    if (i > j)
    {
        int swap = i;
        i = j;
        j = swap;
    }

    while (i < j)
    {
        int temporary = tour[i];
        tour[i] = tour[j];
        tour[j] = temporary;
        ++i;
        --j;
    }
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

Tour anneal(const std::vector<City> &cities, const Tour &start,
            Random &random, const AnnealingOptions &options)
{
    Tour current = start;
    double currentLength = tourLength(cities, current);

    Tour best = current;
    double bestLength = currentLength;

    if (current.size() < 4) return best;

    double temperature = options.startTemperature;

    while (temperature > options.endTemperature)
    {
        int i = 1 + random.below((int)current.size() - 1);
        int j = 1 + random.below((int)current.size() - 1);
        if (i == j)
        {
            temperature *= options.cooling;
            continue;
        }

        Tour candidate = current;
        reverseSegment(candidate, i, j);
        double candidateLength = tourLength(cities, candidate);

        double delta = candidateLength - currentLength;
        bool accept = delta < 0.0;
        if (!accept)
            accept = random.uniform() < std::exp(-delta / temperature);

        if (accept)
        {
            current = candidate;
            currentLength = candidateLength;

            if (currentLength < bestLength)
            {
                best = current;
                bestLength = currentLength;
            }
        }

        temperature *= options.cooling;
    }

    return best;
}
