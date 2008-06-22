#ifndef EDITDISTANCE_H
#define EDITDISTANCE_H

#include <string>
#include <vector>

/* Levenshtein distance: insertions, deletions and substitutions all cost
   one. */
int editDistance(const std::string &a, const std::string &b);

/* Same thing, but gives up as soon as every option is over the limit.
   Useful when scanning a word list where most words are nowhere near. */
int editDistanceWithin(const std::string &a, const std::string &b, int limit);

/* Closest word of the list, or the word itself if it is already there.
   Ties are broken by the order of the list. */
std::string closestWord(const std::string &word,
                        const std::vector<std::string> &dictionary);

#endif
