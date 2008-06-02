#!/usr/bin/env python
"""k nearest neighbours.

No training step at all: every query walks the whole sample set. That is
fine for a few hundred rows, which is all this was written for.
"""

import math


def euclidean(a, b):
    total = 0.0
    for i in range(len(a)):
        d = a[i] - b[i]
        total += d * d
    return math.sqrt(total)


def neighbours(samples, query, k):
    """Return the k closest (distance, label) pairs."""
    distances = []
    for features, label in samples:
        distances.append((euclidean(features, query), label))
    distances.sort()
    return distances[:k]


def classify(samples, query, k=3):
    votes = {}
    for distance, label in neighbours(samples, query, k):
        votes[label] = votes.get(label, 0) + 1

    # ties are broken by the label of the single closest neighbour, which
    # is cheap and good enough
    best = None
    for label, count in votes.items():
        if best is None or count > votes[best]:
            best = label
    if list(votes.values()).count(votes[best]) > 1:
        return neighbours(samples, query, 1)[0][1]
    return best


def leave_one_out(samples, k=3):
    """Accuracy when each row is classified against all the others."""
    right = 0
    for i in range(len(samples)):
        rest = samples[:i] + samples[i + 1:]
        if classify(rest, samples[i][0], k) == samples[i][1]:
            right += 1
    return float(right) / len(samples)


# a handful of rows from the iris set: petal length and petal width
IRIS = [
    ([1.4, 0.2], "setosa"), ([1.4, 0.2], "setosa"), ([1.3, 0.2], "setosa"),
    ([1.5, 0.2], "setosa"), ([1.7, 0.4], "setosa"), ([1.4, 0.3], "setosa"),
    ([4.7, 1.4], "versicolor"), ([4.5, 1.5], "versicolor"), ([4.9, 1.5], "versicolor"),
    ([4.0, 1.3], "versicolor"), ([4.6, 1.5], "versicolor"), ([4.5, 1.3], "versicolor"),
    ([6.0, 2.5], "virginica"), ([5.1, 1.9], "virginica"), ([5.9, 2.1], "virginica"),
    ([5.6, 1.8], "virginica"), ([5.8, 2.2], "virginica"), ([6.6, 2.1], "virginica"),
]


if __name__ == "__main__":
    for k in [1, 3, 5]:
        print("k=%d leave one out accuracy %.2f" % (k, leave_one_out(IRIS, k)))
    print("[5.0, 1.7] looks like %s" % classify(IRIS, [5.0, 1.7]))
