#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "editdistance.h"

namespace {

std::vector<std::string> dictionary()
{
    std::vector<std::string> words;
    words.push_back("kitten");
    words.push_back("sitting");
    words.push_back("saturday");
    words.push_back("sunday");
    words.push_back("algorithm");
    words.push_back("logarithm");
    return words;
}

void testEmptyStrings()
{
    assert(editDistance("", "") == 0);
    assert(editDistance("", "abc") == 3);
    assert(editDistance("abc", "") == 3);
}

void testKnownPairs()
{
    assert(editDistance("kitten", "sitting") == 3);
    assert(editDistance("saturday", "sunday") == 3);
    assert(editDistance("flaw", "lawn") == 2);
    assert(editDistance("algorithm", "logarithm") == 3);
}

void testIdenticalAndSymmetric()
{
    assert(editDistance("banana", "banana") == 0);
    assert(editDistance("banana", "bananas") == editDistance("bananas", "banana"));
    assert(editDistance("abc", "cba") == editDistance("cba", "abc"));
}

void testSingleEdits()
{
    assert(editDistance("cat", "cats") == 1);   /* insertion */
    assert(editDistance("cats", "cat") == 1);   /* deletion */
    assert(editDistance("cat", "cot") == 1);    /* substitution */
}

void testLimitedVersionAgrees()
{
    assert(editDistanceWithin("kitten", "sitting", 5) == 3);

    /* over the limit, the exact value does not matter, only that it is
       reported as too far */
    assert(editDistanceWithin("kitten", "sitting", 2) > 2);
    assert(editDistanceWithin("a", "abcdefgh", 3) > 3);
}

void testClosestWord()
{
    std::vector<std::string> words = dictionary();

    assert(closestWord("kitten", words) == "kitten");
    assert(closestWord("kiten", words) == "kitten");
    assert(closestWord("sundy", words) == "sunday");
    assert(closestWord("algorthm", words) == "algorithm");
    assert(closestWord("anything", std::vector<std::string>()) == "");
}

}

int main()
{
    testEmptyStrings();
    testKnownPairs();
    testIdenticalAndSymmetric();
    testSingleEdits();
    testLimitedVersionAgrees();
    testClosestWord();

    printf("kitten -> sitting costs %d\n", editDistance("kitten", "sitting"));
    printf("did you mean '%s'?\n", closestWord("algorthm", dictionary()).c_str());
    printf("all tests passed\n");
    return 0;
}
