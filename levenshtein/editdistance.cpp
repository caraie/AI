#include "editdistance.h"

#include <cstddef>

namespace {

int minimum(int a, int b, int c)
{
    int best = a;
    if (b < best) best = b;
    if (c < best) best = c;
    return best;
}

}

/*
 * The classic table is (n+1) x (m+1), but each row only looks at the one
 * above, so two rows are enough. For a dictionary scan that difference is
 * the whole point.
 */
int editDistance(const std::string &a, const std::string &b)
{
    const std::size_t rows = a.size();
    const std::size_t columns = b.size();

    if (rows == 0) return (int)columns;
    if (columns == 0) return (int)rows;

    std::vector<int> previous(columns + 1);
    std::vector<int> current(columns + 1);

    for (std::size_t j = 0; j <= columns; ++j)
        previous[j] = (int)j;

    for (std::size_t i = 1; i <= rows; ++i)
    {
        current[0] = (int)i;

        for (std::size_t j = 1; j <= columns; ++j)
        {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            current[j] = minimum(previous[j] + 1,        /* deletion */
                                 current[j - 1] + 1,     /* insertion */
                                 previous[j - 1] + cost); /* substitution */
        }

        previous.swap(current);
    }

    return previous[columns];
}

int editDistanceWithin(const std::string &a, const std::string &b, int limit)
{
    /* The length difference alone is already a lower bound. */
    int difference = (int)a.size() - (int)b.size();
    if (difference < 0) difference = -difference;
    if (difference > limit) return limit + 1;

    int distance = editDistance(a, b);
    if (distance > limit) return limit + 1;
    return distance;
}

std::string closestWord(const std::string &word,
                        const std::vector<std::string> &dictionary)
{
    if (dictionary.empty()) return "";

    std::string best = dictionary[0];
    int bestDistance = editDistance(word, best);

    for (std::size_t i = 1; i < dictionary.size(); ++i)
    {
        if (bestDistance == 0) break;

        int distance = editDistanceWithin(word, dictionary[i], bestDistance);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            best = dictionary[i];
        }
    }

    return best;
}
