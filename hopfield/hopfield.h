#ifndef HOPFIELD_H
#define HOPFIELD_H

#include <string>
#include <vector>

/*
 * Hopfield network used as an associative memory.
 *
 * Neurons are +1 / -1, the weights are symmetric with a zero diagonal and
 * come straight from the Hebb rule. Feeding it a damaged pattern and
 * updating the neurons one at a time walks the state downhill in energy
 * until it falls into the nearest stored pattern.
 *
 * It holds about 0.14 * n patterns before they start blending into each
 * other, so with 25 neurons three is already close to the limit.
 */
class Hopfield
{
public:
    Hopfield(int size);

    /* Patterns are vectors of +1 and -1. */
    void store(const std::vector<int> &pattern);

    /* Updates neurons in order until nothing changes. Returns the number
       of sweeps used. */
    int recall(std::vector<int> &state, int maxSweeps = 20) const;

    double energy(const std::vector<int> &state) const;

    double weight(int i, int j) const { return m_weights[i][j]; }
    int size() const { return m_size; }
    int stored() const { return m_stored; }

private:
    int m_size;
    int m_stored;
    std::vector<std::vector<double> > m_weights;
};

/* Helpers for writing patterns as little pictures: '#' is +1, anything
   else is -1. */
std::vector<int> patternFromRows(const std::vector<std::string> &rows);
std::string patternToText(const std::vector<int> &pattern, int width);
int differences(const std::vector<int> &a, const std::vector<int> &b);

#endif
