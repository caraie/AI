#include "matmul.h"

#include <cmath>

/*
 * Plain three loop version, with k in the middle so that the inner loop
 * walks both b and c along a row. Written for the tests to have something
 * to trust, not to be fast.
 */
void matmulHost(const float *a, const float *b, float *c, int n)
{
    for (int i = 0; i < n * n; ++i)
        c[i] = 0.0f;

    for (int row = 0; row < n; ++row)
    {
        for (int k = 0; k < n; ++k)
        {
            float left = a[row * n + k];
            if (left == 0.0f) continue;

            for (int column = 0; column < n; ++column)
                c[row * n + column] += left * b[k * n + column];
        }
    }
}

void fillMatrix(float *m, int count, unsigned long seed)
{
    unsigned long state = seed;

    for (int i = 0; i < count; ++i)
    {
        state = state * 1103515245UL + 12345UL;
        unsigned long value = (state >> 16) & 0x7fffUL;
        /* small numbers, otherwise the sums grow enough for the float
           rounding to show up in the comparison */
        m[i] = (float)value / 32768.0f - 0.5f;
    }
}

float maxDifference(const float *a, const float *b, int count)
{
    float worst = 0.0f;

    for (int i = 0; i < count; ++i)
    {
        float difference = std::fabs(a[i] - b[i]);
        if (difference > worst) worst = difference;
    }

    return worst;
}
