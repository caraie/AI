#ifndef REGRESSION_H
#define REGRESSION_H

#include <vector>

struct Sample
{
    double x;
    double y;

    Sample() : x(0.0), y(0.0) {}
    Sample(double px, double py) : x(px), y(py) {}
};

/*
 * Simple linear regression, y = slope * x + intercept.
 *
 * There is a closed form for this and it is two lines long, so the
 * gradient descent is here for the exercise: the same loop is what you
 * need once the model has more than one input and the normal equations
 * stop being convenient.
 */
class Regression
{
public:
    Regression() : m_slope(0.0), m_intercept(0.0) {}

    /* Returns the mean squared error at the end of the run. */
    double fit(const std::vector<Sample> &samples, int iterations = 5000,
               double rate = 0.01);

    void fitExact(const std::vector<Sample> &samples);

    double predict(double x) const { return m_slope * x + m_intercept; }
    double cost(const std::vector<Sample> &samples) const;

    double slope() const { return m_slope; }
    double intercept() const { return m_intercept; }

private:
    double m_slope;
    double m_intercept;
};

#endif
