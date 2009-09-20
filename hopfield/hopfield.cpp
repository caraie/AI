#include "hopfield.h"

#include <cstddef>

Hopfield::Hopfield(int size)
    : m_size(size), m_stored(0), m_weights(size, std::vector<double>(size, 0.0))
{
}

void Hopfield::store(const std::vector<int> &pattern)
{
    if ((int)pattern.size() != m_size) return;

    for (int i = 0; i < m_size; ++i)
    {
        for (int j = 0; j < m_size; ++j)
        {
            /* no neuron feeds itself, otherwise every state looks stable */
            if (i == j) continue;
            m_weights[i][j] += (double)(pattern[i] * pattern[j]) / m_size;
        }
    }

    ++m_stored;
}

int Hopfield::recall(std::vector<int> &state, int maxSweeps) const
{
    if ((int)state.size() != m_size) return 0;

    for (int sweep = 1; sweep <= maxSweeps; ++sweep)
    {
        bool changed = false;

        for (int i = 0; i < m_size; ++i)
        {
            double sum = 0.0;
            for (int j = 0; j < m_size; ++j)
                sum += m_weights[i][j] * state[j];

            /* a neuron sitting exactly on the fence keeps its value */
            int value = state[i];
            if (sum > 0.0) value = 1;
            else if (sum < 0.0) value = -1;

            if (value != state[i])
            {
                state[i] = value;
                changed = true;
            }
        }

        if (!changed) return sweep;
    }

    return maxSweeps;
}

double Hopfield::energy(const std::vector<int> &state) const
{
    double total = 0.0;

    for (int i = 0; i < m_size; ++i)
        for (int j = 0; j < m_size; ++j)
            total += m_weights[i][j] * state[i] * state[j];

    return -0.5 * total;
}

std::vector<int> patternFromRows(const std::vector<std::string> &rows)
{
    std::vector<int> pattern;

    for (std::size_t r = 0; r < rows.size(); ++r)
        for (std::size_t c = 0; c < rows[r].size(); ++c)
            pattern.push_back(rows[r][c] == '#' ? 1 : -1);

    return pattern;
}

std::string patternToText(const std::vector<int> &pattern, int width)
{
    std::string text;

    for (std::size_t i = 0; i < pattern.size(); ++i)
    {
        text += (pattern[i] > 0 ? '#' : '.');
        if ((int)(i % width) == width - 1) text += '\n';
    }

    return text;
}

int differences(const std::vector<int> &a, const std::vector<int> &b)
{
    int count = 0;
    for (std::size_t i = 0; i < a.size() && i < b.size(); ++i)
        if (a[i] != b[i])
            ++count;
    return count;
}
