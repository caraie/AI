#!/usr/bin/env python
"""ID3 decision tree.

Only discrete attributes, no pruning and no missing values: the point
was to see the information gain do its job, not to write C4.5.
"""

import math


def entropy(rows, target):
    """Entropy in bits of the target column of these rows."""
    counts = {}
    for row in rows:
        counts[row[target]] = counts.get(row[target], 0) + 1

    total = float(len(rows))
    result = 0.0
    for count in counts.values():
        share = count / total
        result -= share * math.log(share, 2)
    return result


def split(rows, attribute):
    """Group the rows by the value of one attribute."""
    groups = {}
    for row in rows:
        groups.setdefault(row[attribute], []).append(row)
    return groups


def information_gain(rows, attribute, target):
    remainder = 0.0
    for group in split(rows, attribute).values():
        remainder += (float(len(group)) / len(rows)) * entropy(group, target)
    return entropy(rows, target) - remainder


def majority(rows, target):
    counts = {}
    for row in rows:
        counts[row[target]] = counts.get(row[target], 0) + 1

    best = None
    for value, count in counts.items():
        if best is None or count > counts[best]:
            best = value
    return best


def build(rows, attributes, target):
    """Grow the tree. A leaf is a plain value, a node is a dictionary."""
    first = rows[0][target]
    if [row for row in rows if row[target] != first] == []:
        return first

    if not attributes:
        # ran out of questions to ask, go with what most rows say
        return majority(rows, target)

    best = attributes[0]
    best_gain = information_gain(rows, best, target)
    for attribute in attributes[1:]:
        gain = information_gain(rows, attribute, target)
        if gain > best_gain:
            best = attribute
            best_gain = gain

    rest = [a for a in attributes if a != best]
    branches = {}
    for value, group in split(rows, best).items():
        branches[value] = build(group, rest, target)

    return {"attribute": best, "branches": branches}


def classify(tree, sample):
    while isinstance(tree, dict):
        tree = tree["branches"][sample[tree["attribute"]]]
    return tree


def show(tree, indent=""):
    if not isinstance(tree, dict):
        return indent + "-> " + str(tree) + "\n"

    text = ""
    for value, subtree in tree["branches"].items():
        text += "%s%s = %s\n" % (indent, tree["attribute"], value)
        text += show(subtree, indent + "    ")
    return text


# the play tennis table from Quinlan's paper
ATTRIBUTES = ["outlook", "temperature", "humidity", "wind"]

TENNIS = [
    {"outlook": "sunny", "temperature": "hot", "humidity": "high", "wind": "weak", "play": "no"},
    {"outlook": "sunny", "temperature": "hot", "humidity": "high", "wind": "strong", "play": "no"},
    {"outlook": "overcast", "temperature": "hot", "humidity": "high", "wind": "weak", "play": "yes"},
    {"outlook": "rain", "temperature": "mild", "humidity": "high", "wind": "weak", "play": "yes"},
    {"outlook": "rain", "temperature": "cool", "humidity": "normal", "wind": "weak", "play": "yes"},
    {"outlook": "rain", "temperature": "cool", "humidity": "normal", "wind": "strong", "play": "no"},
    {"outlook": "overcast", "temperature": "cool", "humidity": "normal", "wind": "strong", "play": "yes"},
    {"outlook": "sunny", "temperature": "mild", "humidity": "high", "wind": "weak", "play": "no"},
    {"outlook": "sunny", "temperature": "cool", "humidity": "normal", "wind": "weak", "play": "yes"},
    {"outlook": "rain", "temperature": "mild", "humidity": "normal", "wind": "weak", "play": "yes"},
    {"outlook": "sunny", "temperature": "mild", "humidity": "normal", "wind": "strong", "play": "yes"},
    {"outlook": "overcast", "temperature": "mild", "humidity": "high", "wind": "strong", "play": "yes"},
    {"outlook": "overcast", "temperature": "hot", "humidity": "normal", "wind": "weak", "play": "yes"},
    {"outlook": "rain", "temperature": "mild", "humidity": "high", "wind": "strong", "play": "no"},
]


if __name__ == "__main__":
    tree = build(TENNIS, ATTRIBUTES, "play")
    print(show(tree))
    for attribute in ATTRIBUTES:
        print("gain(%s) = %.3f" % (attribute, information_gain(TENNIS, attribute, "play")))
