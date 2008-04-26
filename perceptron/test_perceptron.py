#!/usr/bin/env python
"""Tests for the perceptron."""

import unittest

import perceptron
from perceptron import Perceptron


class LogicGateTest(unittest.TestCase):

    def test_learns_and(self):
        net = Perceptron(2)
        net.train(perceptron.AND_SAMPLES, perceptron.AND_LABELS)
        self.assertEqual(1.0, net.accuracy(perceptron.AND_SAMPLES,
                                           perceptron.AND_LABELS))

    def test_learns_or(self):
        net = Perceptron(2)
        net.train(perceptron.AND_SAMPLES, perceptron.OR_LABELS)
        self.assertEqual(1.0, net.accuracy(perceptron.AND_SAMPLES,
                                           perceptron.OR_LABELS))

    def test_gives_up_on_xor(self):
        # xor is not linearly separable, one line cannot cut it
        net = Perceptron(2)
        used = net.train(perceptron.AND_SAMPLES, perceptron.XOR_LABELS, 200)
        self.assertEqual(200, used)
        self.assertTrue(net.accuracy(perceptron.AND_SAMPLES,
                                     perceptron.XOR_LABELS) < 1.0)


class SeparableTest(unittest.TestCase):

    def setUp(self):
        # two clusters, one around (1,1) and the other around (5,5)
        self.samples = [[1, 1], [1, 2], [2, 1], [2, 2],
                        [5, 5], [5, 6], [6, 5], [6, 6]]
        self.labels = [-1, -1, -1, -1, 1, 1, 1, 1]

    def test_separates_the_clusters(self):
        net = Perceptron(2)
        net.train(self.samples, self.labels)
        self.assertEqual(1.0, net.accuracy(self.samples, self.labels))

    def test_classifies_a_new_point(self):
        net = Perceptron(2)
        net.train(self.samples, self.labels)
        self.assertEqual(1, net.output([7, 7]))
        self.assertEqual(-1, net.output([0, 0]))


if __name__ == "__main__":
    unittest.main()
