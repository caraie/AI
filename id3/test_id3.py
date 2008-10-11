#!/usr/bin/env python
"""Tests for the ID3 tree."""

import unittest

import id3


class EntropyTest(unittest.TestCase):

    def test_pure_set_is_zero(self):
        rows = [{"t": "yes"}, {"t": "yes"}]
        self.assertEqual(0.0, id3.entropy(rows, "t"))

    def test_even_split_is_one_bit(self):
        rows = [{"t": "yes"}, {"t": "no"}]
        self.assertAlmostEqual(1.0, id3.entropy(rows, "t"))

    def test_tennis_table(self):
        # nine yes and five no
        self.assertAlmostEqual(0.940, id3.entropy(id3.TENNIS, "play"), 3)


class GainTest(unittest.TestCase):

    def test_outlook_is_the_best_question(self):
        gains = {}
        for attribute in id3.ATTRIBUTES:
            gains[attribute] = id3.information_gain(id3.TENNIS, attribute, "play")
        best = max(gains.values())
        self.assertEqual(gains["outlook"], best)
        self.assertAlmostEqual(0.247, gains["outlook"], 3)

    def test_useless_attribute_gains_nothing(self):
        rows = [{"a": "x", "t": "yes"}, {"a": "y", "t": "yes"}]
        self.assertAlmostEqual(0.0, id3.information_gain(rows, "a", "t"))


class TreeTest(unittest.TestCase):

    def setUp(self):
        self.tree = id3.build(id3.TENNIS, id3.ATTRIBUTES, "play")

    def test_root_splits_on_outlook(self):
        self.assertEqual("outlook", self.tree["attribute"])

    def test_overcast_is_always_yes(self):
        self.assertEqual("yes", self.tree["branches"]["overcast"])

    def test_reproduces_the_training_set(self):
        for row in id3.TENNIS:
            self.assertEqual(row["play"], id3.classify(self.tree, row))

    def test_unseen_value_falls_back_to_the_majority(self):
        # no row in the table was ever taken in the snow
        day = {"outlook": "snow", "temperature": "cool",
               "humidity": "normal", "wind": "weak"}
        self.assertEqual("yes", id3.classify(self.tree, day))

    def test_classifies_a_new_day(self):
        day = {"outlook": "sunny", "temperature": "cool",
               "humidity": "normal", "wind": "strong"}
        self.assertEqual("yes", id3.classify(self.tree, day))


if __name__ == "__main__":
    unittest.main()
