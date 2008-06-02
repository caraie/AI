#!/usr/bin/env python
"""Tests for k nearest neighbours."""

import unittest

import knn


SAMPLES = [([0.0, 0.0], "a"), ([0.5, 0.2], "a"), ([0.1, 0.4], "a"),
           ([5.0, 5.0], "b"), ([5.2, 4.8], "b"), ([4.9, 5.3], "b")]


class DistanceTest(unittest.TestCase):

    def test_zero_for_the_same_point(self):
        self.assertEqual(0.0, knn.euclidean([1.0, 2.0], [1.0, 2.0]))

    def test_pythagoras(self):
        self.assertAlmostEqual(5.0, knn.euclidean([0.0, 0.0], [3.0, 4.0]))


class ClassifyTest(unittest.TestCase):

    def test_picks_the_near_cluster(self):
        self.assertEqual("a", knn.classify(SAMPLES, [0.2, 0.2]))
        self.assertEqual("b", knn.classify(SAMPLES, [5.0, 5.1]))

    def test_k_of_one_is_the_closest_row(self):
        self.assertEqual("b", knn.classify(SAMPLES, [4.9, 5.3], 1))

    def test_neighbours_come_back_sorted(self):
        found = knn.neighbours(SAMPLES, [0.0, 0.0], 3)
        self.assertEqual(3, len(found))
        self.assertTrue(found[0][0] <= found[1][0] <= found[2][0])

    def test_iris_rows_are_mostly_right(self):
        self.assertTrue(knn.leave_one_out(knn.IRIS, 3) > 0.85)


if __name__ == "__main__":
    unittest.main()
