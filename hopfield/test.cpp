#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "hopfield.h"

namespace {

std::vector<std::string> rowsOf(const char *a, const char *b, const char *c,
                                const char *d, const char *e)
{
    std::vector<std::string> rows;
    rows.push_back(a);
    rows.push_back(b);
    rows.push_back(c);
    rows.push_back(d);
    rows.push_back(e);
    return rows;
}

std::vector<int> letterT()
{
    return patternFromRows(rowsOf("#####",
                                  "..#..",
                                  "..#..",
                                  "..#..",
                                  "..#.."));
}

std::vector<int> letterL()
{
    return patternFromRows(rowsOf("#....",
                                  "#....",
                                  "#....",
                                  "#....",
                                  "#####"));
}

std::vector<int> letterO()
{
    return patternFromRows(rowsOf("#####",
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  "#####"));
}

Hopfield trained()
{
    Hopfield network(25);
    network.store(letterT());
    network.store(letterL());
    network.store(letterO());
    return network;
}

/* Flips the first n cells of a pattern, which is as good a way as any of
   damaging it in a repeatable manner. */
std::vector<int> damage(const std::vector<int> &pattern, int cells)
{
    std::vector<int> copy = pattern;
    for (int i = 0; i < cells && i < (int)copy.size(); ++i)
        copy[i] = -copy[i];
    return copy;
}

void testPatternHelpers()
{
    std::vector<int> pattern = letterT();
    assert(pattern.size() == 25);
    assert(pattern[0] == 1);
    assert(pattern[5] == -1);
    assert(pattern[7] == 1);

    assert(differences(letterT(), letterT()) == 0);
    assert(differences(letterT(), letterL()) > 0);
}

void testWeightsAreSymmetric()
{
    Hopfield network = trained();

    assert(network.stored() == 3);
    for (int i = 0; i < network.size(); ++i)
    {
        assert(network.weight(i, i) == 0.0);
        for (int j = 0; j < network.size(); ++j)
            assert(network.weight(i, j) == network.weight(j, i));
    }
}

void testStoredPatternsAreStable()
{
    Hopfield network = trained();

    std::vector<int> state = letterT();
    int sweeps = network.recall(state);
    assert(sweeps == 1);
    assert(differences(state, letterT()) == 0);

    state = letterL();
    network.recall(state);
    assert(differences(state, letterL()) == 0);
}

void testRecallsFromDamage()
{
    Hopfield network = trained();

    std::vector<int> state = damage(letterT(), 4);
    assert(differences(state, letterT()) == 4);

    network.recall(state);
    assert(differences(state, letterT()) == 0);
}

void testEnergyGoesDown()
{
    Hopfield network = trained();

    std::vector<int> state = damage(letterO(), 3);
    double before = network.energy(state);

    network.recall(state);
    double after = network.energy(state);

    assert(after <= before);
    assert(differences(state, letterO()) == 0);
}

void testStoredPatternSitsInAMinimum()
{
    Hopfield network = trained();

    std::vector<int> pattern = letterL();
    double bottom = network.energy(pattern);

    /* every single flip should make the state worse */
    for (int i = 0; i < 25; ++i)
    {
        std::vector<int> neighbour = pattern;
        neighbour[i] = -neighbour[i];
        assert(network.energy(neighbour) >= bottom);
    }
}

void testWrongSizedPatternIsIgnored()
{
    Hopfield network(25);
    std::vector<int> tooShort(10, 1);

    network.store(tooShort);
    assert(network.stored() == 0);
    assert(network.recall(tooShort) == 0);
}

}

int main()
{
    testPatternHelpers();
    testWeightsAreSymmetric();
    testStoredPatternsAreStable();
    testRecallsFromDamage();
    testEnergyGoesDown();
    testStoredPatternSitsInAMinimum();
    testWrongSizedPatternIsIgnored();

    Hopfield network = trained();
    std::vector<int> state = damage(letterT(), 4);

    printf("damaged input:\n%s\n", patternToText(state, 5).c_str());
    int sweeps = network.recall(state);
    printf("after %d sweeps:\n%s\n", sweeps, patternToText(state, 5).c_str());
    printf("all tests passed\n");
    return 0;
}
