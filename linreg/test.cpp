#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "regression.h"

namespace {

bool close(double a, double b, double tolerance)
{
    return std::fabs(a - b) < tolerance;
}

/* y = 2x + 1 with a little noise added by hand, so the numbers stay the
   same on every machine. */
std::vector<Sample> noisyLine()
{
    std::vector<Sample> samples;
    samples.push_back(Sample(0.0, 1.1));
    samples.push_back(Sample(1.0, 2.9));
    samples.push_back(Sample(2.0, 5.2));
    samples.push_back(Sample(3.0, 6.8));
    samples.push_back(Sample(4.0, 9.1));
    samples.push_back(Sample(5.0, 10.9));
    samples.push_back(Sample(6.0, 13.2));
    samples.push_back(Sample(7.0, 14.8));
    return samples;
}

void testExactLine()
{
    std::vector<Sample> samples;
    for (int i = 0; i < 5; ++i)
        samples.push_back(Sample(i, 3.0 * i - 2.0));

    Regression model;
    model.fitExact(samples);

    assert(close(model.slope(), 3.0, 1e-9));
    assert(close(model.intercept(), -2.0, 1e-9));
    assert(close(model.cost(samples), 0.0, 1e-9));
}

void testDescentFindsTheLine()
{
    std::vector<Sample> samples = noisyLine();

    Regression model;
    model.fit(samples, 20000, 0.01);

    assert(close(model.slope(), 2.0, 0.05));
    assert(close(model.intercept(), 1.0, 0.15));
}

void testDescentAgreesWithLeastSquares()
{
    std::vector<Sample> samples = noisyLine();

    Regression descent;
    descent.fit(samples, 20000, 0.01);

    Regression exact;
    exact.fitExact(samples);

    assert(close(descent.slope(), exact.slope(), 1e-4));
    assert(close(descent.intercept(), exact.intercept(), 1e-3));
}

void testMoreIterationsCostLess()
{
    std::vector<Sample> samples = noisyLine();

    Regression few, many;
    double firstCost = few.fit(samples, 10, 0.01);
    double secondCost = many.fit(samples, 1000, 0.01);

    assert(secondCost < firstCost);
}

void testPredict()
{
    std::vector<Sample> samples = noisyLine();

    Regression model;
    model.fitExact(samples);

    /* the line goes through the middle of the cloud */
    assert(close(model.predict(3.5), 8.0, 0.3));
}

void testEmptyInput()
{
    std::vector<Sample> empty;

    Regression model;
    assert(model.fit(empty) == 0.0);
    assert(model.cost(empty) == 0.0);
    assert(model.slope() == 0.0);
}

void testVerticalCloudIsIgnored()
{
    /* every x is 2, there is no slope to compute */
    std::vector<Sample> samples;
    samples.push_back(Sample(2.0, 1.0));
    samples.push_back(Sample(2.0, 5.0));

    Regression model;
    model.fitExact(samples);
    assert(model.slope() == 0.0);
}

}

int main()
{
    testExactLine();
    testDescentFindsTheLine();
    testDescentAgreesWithLeastSquares();
    testMoreIterationsCostLess();
    testPredict();
    testEmptyInput();
    testVerticalCloudIsIgnored();

    std::vector<Sample> samples = noisyLine();
    Regression model;
    double error = model.fit(samples, 20000, 0.01);
    printf("y = %.4f x + %.4f, mean squared error %.5f\n",
           model.slope(), model.intercept(), error);
    printf("all tests passed\n");
    return 0;
}
