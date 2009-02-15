#!/usr/bin/env python
"""Viterbi decoding for a hidden markov model.

Finds the most likely sequence of hidden states behind a sequence of
observations. Everything is kept in plain dictionaries so the model can
be written by hand and read back.
"""


def decode(observations, states, start, transition, emission):
    """Return (probability, path) of the most likely state sequence.

    start[s]           probability of starting in s
    transition[s][t]   probability of going from s to t
    emission[s][o]     probability of seeing o while in s
    """
    if not observations:
        return 0.0, []

    # table[t][state] = (probability of the best path ending here, path)
    table = [{}]
    for state in states:
        table[0][state] = (start[state] * emission[state][observations[0]], [state])

    for i in range(1, len(observations)):
        table.append({})
        for state in states:
            best_probability = -1.0
            best_path = None

            for previous in states:
                probability, path = table[i - 1][previous]
                probability = probability * transition[previous][state] * \
                    emission[state][observations[i]]
                if probability > best_probability:
                    best_probability = probability
                    best_path = path + [state]

            table[i][state] = (best_probability, best_path)

    # of all the paths that end anywhere, keep the best one
    best = None
    for state in states:
        if best is None or table[-1][state][0] > best[0]:
            best = table[-1][state]
    return best


# the doctor example: the patient's health is hidden, only the symptom
# reported each day is visible
STATES = ["healthy", "fever"]
OBSERVATIONS = ["normal", "cold", "dizzy"]

START = {"healthy": 0.6, "fever": 0.4}

TRANSITION = {
    "healthy": {"healthy": 0.7, "fever": 0.3},
    "fever": {"healthy": 0.4, "fever": 0.6},
}

EMISSION = {
    "healthy": {"normal": 0.5, "cold": 0.4, "dizzy": 0.1},
    "fever": {"normal": 0.1, "cold": 0.3, "dizzy": 0.6},
}


if __name__ == "__main__":
    probability, path = decode(OBSERVATIONS, STATES, START, TRANSITION, EMISSION)
    for i in range(len(OBSERVATIONS)):
        print("%-8s -> %s" % (OBSERVATIONS[i], path[i]))
    print("probability %.6f" % probability)
