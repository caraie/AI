#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "random.h"
#include "tsp.h"

namespace {

Tour identityTour(int count)
{
    Tour tour;
    for (int i = 0; i < count; ++i)
        tour.push_back(i);
    return tour;
}

/* A tour is only valid if every city shows up exactly once. */
bool isPermutation(const Tour &tour, int count)
{
    if ((int)tour.size() != count) return false;

    std::vector<bool> seen(count, false);
    for (std::size_t i = 0; i < tour.size(); ++i)
    {
        if (tour[i] < 0 || tour[i] >= count) return false;
        if (seen[tour[i]]) return false;
        seen[tour[i]] = true;
    }

    return true;
}

void testTourLength()
{
    std::vector<City> square;
    square.push_back(City(0, 0));
    square.push_back(City(1, 0));
    square.push_back(City(1, 1));
    square.push_back(City(0, 1));

    assert(std::fabs(tourLength(square, identityTour(4)) - 4.0) < 1e-9);

    Tour single;
    single.push_back(0);
    assert(tourLength(square, single) == 0.0);
}

void testReverseSegment()
{
    Tour tour = identityTour(6);
    reverseSegment(tour, 1, 4);

    assert(tour[0] == 0);
    assert(tour[1] == 4);
    assert(tour[2] == 3);
    assert(tour[3] == 2);
    assert(tour[4] == 1);
    assert(tour[5] == 5);

    /* the arguments can come in any order */
    Tour other = identityTour(6);
    reverseSegment(other, 4, 1);
    assert(other == tour);
}

void testCrossedSquareIsLonger()
{
    std::vector<City> square;
    square.push_back(City(0, 0));
    square.push_back(City(1, 0));
    square.push_back(City(1, 1));
    square.push_back(City(0, 1));

    Tour crossed;
    crossed.push_back(0);
    crossed.push_back(2);
    crossed.push_back(1);
    crossed.push_back(3);

    assert(tourLength(square, crossed) > tourLength(square, identityTour(4)));
}

void testAnnealingKeepsAValidTour()
{
    std::vector<City> cities = circleOfCities(12, 10.0);
    Random random(5);

    Tour result = anneal(cities, identityTour(12), random);
    assert(isPermutation(result, 12));
}

void testAnnealingUncrossesTheCircle()
{
    const int count = 12;
    std::vector<City> cities = circleOfCities(count, 10.0);

    /* start from a deliberately tangled tour */
    Tour tangled;
    for (int i = 0; i < count; i += 2) tangled.push_back(i);
    for (int i = 1; i < count; i += 2) tangled.push_back(i);

    Random random(5);
    Tour result = anneal(cities, tangled, random);

    double before = tourLength(cities, tangled);
    double after = tourLength(cities, result);
    double best = tourLength(cities, identityTour(count));

    assert(after < before);
    assert(after < best * 1.05);
}

void testTinyProblemIsLeftAlone()
{
    std::vector<City> cities = circleOfCities(3, 1.0);
    Random random(1);

    Tour result = anneal(cities, identityTour(3), random);
    assert(result == identityTour(3));
}

}

int main()
{
    testTourLength();
    testReverseSegment();
    testCrossedSquareIsLonger();
    testAnnealingKeepsAValidTour();
    testAnnealingUncrossesTheCircle();
    testTinyProblemIsLeftAlone();

    std::vector<City> cities = circleOfCities(20, 10.0);
    Tour start = identityTour(20);
    reverseSegment(start, 3, 15);
    Random random(2009);

    Tour result = anneal(cities, start, random);
    printf("start %.3f, annealed %.3f, best possible %.3f\n",
           tourLength(cities, start), tourLength(cities, result),
           tourLength(cities, identityTour(20)));
    printf("all tests passed\n");
    return 0;
}
