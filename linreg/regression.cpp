#include "regression.h"

#include <cstddef>

double Regression::cost(const std::vector<Sample> &samples) const
{
    if (samples.empty()) return 0.0;

    double total = 0.0;
    for (std::size_t i = 0; i < samples.size(); ++i)
    {
        double error = predict(samples[i].x) - samples[i].y;
        total += error * error;
    }

    return total / samples.size();
}

double Regression::fit(const std::vector<Sample> &samples, int iterations,
                       double rate)
{
    if (samples.empty()) return 0.0;

    m_slope = 0.0;
    m_intercept = 0.0;

    const double n = (double)samples.size();

    for (int step = 0; step < iterations; ++step)
    {
        double slopeGradient = 0.0;
        double interceptGradient = 0.0;

        for (std::size_t i = 0; i < samples.size(); ++i)
        {
            double error = predict(samples[i].x) - samples[i].y;
            slopeGradient += error * samples[i].x;
            interceptGradient += error;
        }

        /* both partial derivatives carry a 2/n, keep it so that the rate
           means the same thing whatever the size of the set */
        m_slope -= rate * 2.0 * slopeGradient / n;
        m_intercept -= rate * 2.0 * interceptGradient / n;
    }

    return cost(samples);
}

/* Least squares, for checking that the descent lands where it should. */
void Regression::fitExact(const std::vector<Sample> &samples)
{
    if (samples.empty()) return;

    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    const double n = (double)samples.size();

    for (std::size_t i = 0; i < samples.size(); ++i)
    {
        sumX += samples[i].x;
        sumY += samples[i].y;
        sumXY += samples[i].x * samples[i].y;
        sumXX += samples[i].x * samples[i].x;
    }

    double denominator = n * sumXX - sumX * sumX;
    if (denominator == 0.0) return;  /* every x is the same, no line to fit */

    m_slope = (n * sumXY - sumX * sumY) / denominator;
    m_intercept = (sumY - m_slope * sumX) / n;
}
