#ifndef ACO_H
#define ACO_H

#include <vector>

#include "random.h"

struct City
{
    double x;
    double y;

    City() : x(0.0), y(0.0) {}
    City(double px, double py) : x(px), y(py) {}
};

typedef std::vector<int> Tour;

double tourLength(const std::vector<City> &cities, const Tour &tour);
std::vector<City> circleOfCities(int count, double radius);

struct AcoOptions
{
    int ants;
    int iterations;
    double alpha;        /* weight of the trail */
    double beta;         /* weight of the distance */
    double evaporation;  /* share of pheromone lost each round */
    int elite;           /* extra deposits on the best tour so far */

    AcoOptions()
        : ants(20), iterations(120), alpha(1.0), beta(3.0),
          evaporation(0.5), elite(4) {}
};

/*
 * Ant colony optimisation for the travelling salesman problem.
 *
 * Every ant walks a tour choosing the next city with probability
 * proportional to trail^alpha * (1/distance)^beta, and then lays down
 * pheromone in inverse proportion to the length of what it walked. Short
 * tours get reinforced, the rest evaporates.
 *
 * On top of that the best tour found so far is reinforced a few extra
 * times every round. Without it the trail spreads too thin on the bigger
 * rings and the colony wanders for a long time.
 */
class AntColony
{
public:
    AntColony(const std::vector<City> &cities, const AcoOptions &options = AcoOptions());

    Tour run(Random &random);

    double bestLength() const { return m_bestLength; }
    const std::vector<std::vector<double> > &trails() const { return m_trails; }

private:
    Tour walk(Random &random) const;
    int chooseNext(int current, const std::vector<bool> &visited, Random &random) const;
    void evaporate();
    void deposit(const Tour &tour, double length);

    std::vector<City> m_cities;
    AcoOptions m_options;
    std::vector<std::vector<double> > m_trails;
    std::vector<std::vector<double> > m_distances;
    Tour m_best;
    double m_bestLength;
};

#endif
