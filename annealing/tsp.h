#ifndef TSP_H
#define TSP_H

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

/* Reverses the stretch between i and j, the usual 2-opt move. */
void reverseSegment(Tour &tour, int i, int j);

/* Cities placed on a circle: handy for testing, since the best tour is
   just going round in order. */
std::vector<City> circleOfCities(int count, double radius);

struct AnnealingOptions
{
    double startTemperature;
    double endTemperature;
    double cooling;      /* multiplied into the temperature each step */

    AnnealingOptions()
        : startTemperature(100.0), endTemperature(0.01), cooling(0.9995) {}
};

/*
 * Simulated annealing over 2-opt moves. A worse tour is still taken with
 * probability exp(-delta / temperature), which is what lets it climb out
 * of a local minimum early on.
 */
Tour anneal(const std::vector<City> &cities, const Tour &start,
            Random &random, const AnnealingOptions &options = AnnealingOptions());

#endif
