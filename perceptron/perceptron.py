#!/usr/bin/env python
"""Rosenblatt's perceptron.

Binary classifier with labels +1 and -1. Only works when the data is
linearly separable; when it is not the weights keep bouncing around and
the training just runs out of epochs.
"""

import random


def dot(a, b):
    total = 0.0
    for i in range(len(a)):
        total += a[i] * b[i]
    return total


class Perceptron:

    def __init__(self, size, rate=0.1):
        # one extra weight for the bias, kept as weight zero with a
        # constant input of 1
        self.weights = [0.0] * (size + 1)
        self.rate = rate

    def output(self, sample):
        activation = self.weights[0] + dot(self.weights[1:], sample)
        if activation >= 0.0:
            return 1
        return -1

    def train(self, samples, labels, epochs=100, rng=None):
        """Run the update rule until nothing is misclassified.

        Returns the number of epochs used, or epochs if it never
        settled down.

        The order of the samples is shuffled on every pass. Walking them
        always in the same order lets the last few examples pull the
        weights around more than the rest.
        """
        if rng is None:
            rng = random.Random()

        order = list(range(len(samples)))

        for epoch in range(epochs):
            rng.shuffle(order)
            errors = 0
            for i in order:
                sample = samples[i]
                guess = self.output(sample)
                if guess == labels[i]:
                    continue
                errors += 1
                delta = self.rate * (labels[i] - guess)
                self.weights[0] += delta
                for j in range(len(sample)):
                    self.weights[j + 1] += delta * sample[j]
            if errors == 0:
                return epoch + 1
        return epochs

    def accuracy(self, samples, labels):
        right = 0
        for i in range(len(samples)):
            if self.output(samples[i]) == labels[i]:
                right += 1
        return float(right) / len(samples)


AND_SAMPLES = [[0, 0], [0, 1], [1, 0], [1, 1]]
AND_LABELS = [-1, -1, -1, 1]

OR_LABELS = [-1, 1, 1, 1]
XOR_LABELS = [-1, 1, 1, -1]


if __name__ == "__main__":
    for name, labels in [("and", AND_LABELS), ("or", OR_LABELS), ("xor", XOR_LABELS)]:
        net = Perceptron(2)
        epochs = net.train(AND_SAMPLES, labels, 50)
        print("%s: %d epochs, accuracy %.2f" %
              (name, epochs, net.accuracy(AND_SAMPLES, labels)))
