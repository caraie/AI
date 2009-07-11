#include "fuzzy.h"

#include <cstddef>

double Triangle::membership(double value) const
{
    if (value <= m_left || value >= m_right) return 0.0;

    if (value == m_peak) return 1.0;

    if (value < m_peak)
        return (value - m_left) / (m_peak - m_left);

    return (m_right - value) / (m_right - m_peak);
}

void Controller::add(const Rule &rule)
{
    m_rules.push_back(rule);
}

std::vector<double> Controller::strengths(double input) const
{
    std::vector<double> result;
    for (std::size_t i = 0; i < m_rules.size(); ++i)
        result.push_back(m_rules[i].condition.membership(input));
    return result;
}

double Controller::evaluate(double input) const
{
    double weighted = 0.0;
    double total = 0.0;

    for (std::size_t i = 0; i < m_rules.size(); ++i)
    {
        double strength = m_rules[i].condition.membership(input);
        weighted += strength * m_rules[i].output;
        total += strength;
    }

    /* Outside every set no rule has anything to say. Returning zero would
       mean "fan off", which is a different statement, so hold the nearest
       rule's output instead. */
    if (total == 0.0)
    {
        double best = 0.0;
        double bestDistance = -1.0;

        for (std::size_t i = 0; i < m_rules.size(); ++i)
        {
            double distance = input - m_rules[i].condition.peak();
            if (distance < 0.0) distance = -distance;
            if (bestDistance < 0.0 || distance < bestDistance)
            {
                bestDistance = distance;
                best = m_rules[i].output;
            }
        }

        return best;
    }

    return weighted / total;
}

Controller fanController()
{
    Controller controller;
    controller.add(Rule("cold", Triangle(-100.0, 10.0, 18.0), 0.0));
    controller.add(Rule("mild", Triangle(14.0, 21.0, 26.0), 35.0));
    controller.add(Rule("warm", Triangle(22.0, 28.0, 34.0), 70.0));
    controller.add(Rule("hot", Triangle(30.0, 38.0, 100.0), 100.0));
    return controller;
}
