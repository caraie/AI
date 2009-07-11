#ifndef FUZZY_H
#define FUZZY_H

#include <string>
#include <vector>

/*
 * Triangular membership function. Left and right are where the membership
 * is zero, peak is where it is one. A shoulder is written by pushing the
 * outer point far away.
 */
class Triangle
{
public:
    Triangle() : m_left(0.0), m_peak(0.0), m_right(0.0) {}
    Triangle(double left, double peak, double right)
        : m_left(left), m_peak(peak), m_right(right) {}

    double membership(double value) const;

    double peak() const { return m_peak; }

private:
    double m_left;
    double m_peak;
    double m_right;
};

/* A rule reads "if temperature is <set> then fan is <output>". */
struct Rule
{
    std::string name;
    Triangle condition;
    double output;   /* the fan speed this rule votes for */

    Rule() : output(0.0) {}
    Rule(const std::string &n, const Triangle &c, double o)
        : name(n), condition(c), output(o) {}
};

/*
 * Sugeno style controller: each rule fires with the strength of its
 * membership and the answer is the weighted average of the outputs. The
 * Mamdani version needs the output sets defuzzified, which is more work
 * for the same shape of answer on a problem this small.
 */
class Controller
{
public:
    void add(const Rule &rule);

    double evaluate(double input) const;

    /* Strength of every rule for this input, in the order they were added. */
    std::vector<double> strengths(double input) const;

    const std::vector<Rule> &rules() const { return m_rules; }

private:
    std::vector<Rule> m_rules;
};

/* The example controller: room temperature in degrees, fan speed in
   percent. */
Controller fanController();

#endif
