#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "aco.h"
#include "random.h"

namespace {

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

Tour inOrder(int count)
{
    Tour tour;
    for (int i = 0; i < count; ++i)
        tour.push_back(i);
    return tour;
}

void testTourLength()
{
    std::vector<City> square;
    square.push_back(City(0, 0));
    square.push_back(City(2, 0));
    square.push_back(City(2, 2));
    square.push_back(City(0, 2));

    assert(std::fabs(tourLength(square, inOrder(4)) - 8.0) < 1e-9);
}

void testCircleGeometry()
{
    std::vector<City> cities = circleOfCities(4, 1.0);

    assert(cities.size() == 4);
    assert(std::fabs(cities[0].x - 1.0) < 1e-9);
    assert(std::fabs(cities[0].y) < 1e-9);
    assert(std::fabs(cities[2].x + 1.0) < 1e-9);
}

void testWalkVisitsEveryCity()
{
    std::vector<City> cities = circleOfCities(10, 5.0);

    AcoOptions options;
    options.ants = 4;
    options.iterations = 2;

    AntColony colony(cities, options);
    Random random(3);

    Tour tour = colony.run(random);
    assert(isPermutation(tour, 10));
}

void testFindsTheCircle()
{
    const int count = 10;
    std::vector<City> cities = circleOfCities(count, 10.0);

    AntColony colony(cities);
    Random random(2009);
    Tour tour = colony.run(random);

    /* the best tour on a circle is simply going round it */
    double best = tourLength(cities, inOrder(count));
    assert(isPermutation(tour, count));
    assert(colony.bestLength() < best * 1.02);
}

void testTrailsGrowOnGoodEdges()
{
    std::vector<City> cities = circleOfCities(8, 4.0);

    AntColony colony(cities);
    Random random(11);
    colony.run(random);

    /* neighbours on the circle should end up with a stronger trail than
       the two cities on opposite sides of it */
    double neighbour = colony.trails()[0][1];
    double across = colony.trails()[0][4];
    assert(neighbour > across);
}

void testEliteAntsDoNotHurt()
{
    const int count = 20;
    std::vector<City> cities = circleOfCities(count, 10.0);
    double best = tourLength(cities, inOrder(count));

    AcoOptions plain;
    plain.elite = 0;

    AntColony without(cities, plain);
    Random first(1979);
    without.run(first);

    AntColony with(cities);
    Random second(1979);
    with.run(second);

    assert(with.bestLength() <= without.bestLength());
    assert(with.bestLength() < best * 1.02);
}

void testTwoCities()
{
    std::vector<City> cities;
    cities.push_back(City(0, 0));
    cities.push_back(City(3, 4));

    AntColony colony(cities);
    Random random(1);
    Tour tour = colony.run(random);

    assert(isPermutation(tour, 2));
    assert(std::fabs(colony.bestLength() - 10.0) < 1e-9);
}

}

int main()
{
    testTourLength();
    testCircleGeometry();
    testWalkVisitsEveryCity();
    testFindsTheCircle();
    testTrailsGrowOnGoodEdges();
    testEliteAntsDoNotHurt();
    testTwoCities();

    std::vector<City> cities = circleOfCities(15, 10.0);
    AntColony colony(cities);
    Random random(2009);
    colony.run(random);

    printf("15 cities: colony found %.3f, best possible %.3f\n",
           colony.bestLength(), tourLength(cities, inOrder(15)));
    printf("all tests passed\n");
    return 0;
}
