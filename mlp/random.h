#ifndef RANDOM_H
#define RANDOM_H

/*
 * Small linear congruential generator. rand() is good enough for this but
 * it gives different numbers on different machines, and a training run
 * that cannot be repeated is a pain to debug.
 */
class Random
{
public:
    Random(unsigned long seed = 1) : m_state(seed) {}

    unsigned long next()
    {
        m_state = m_state * 1103515245UL + 12345UL;
        return (m_state >> 16) & 0x7fffUL;
    }

    /* Uniform in [0, 1). */
    double uniform() { return next() / 32768.0; }

    /* Uniform in [low, high). */
    double between(double low, double high)
    {
        return low + uniform() * (high - low);
    }

    int below(int limit) { return (int)(uniform() * limit); }

private:
    unsigned long m_state;
};

#endif
