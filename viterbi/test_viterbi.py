#!/usr/bin/env python
"""Tests for the viterbi decoder."""

import unittest

import viterbi
from viterbi import STATES, START, TRANSITION, EMISSION


def decode(observations):
    return viterbi.decode(observations, STATES, START, TRANSITION, EMISSION)


class DecodeTest(unittest.TestCase):

    def test_empty_input(self):
        self.assertEqual((0.0, []), decode([]))

    def test_single_observation(self):
        probability, path = decode(["dizzy"])
        self.assertEqual(["fever"], path)
        # 0.4 * 0.6, no transition involved yet
        self.assertAlmostEqual(0.24, probability)

    def test_the_doctor_example(self):
        probability, path = decode(["normal", "cold", "dizzy"])
        self.assertEqual(["healthy", "healthy", "fever"], path)
        self.assertAlmostEqual(0.01512, probability, 5)

    def test_path_has_one_state_per_observation(self):
        observations = ["normal", "normal", "cold", "dizzy", "dizzy"]
        probability, path = decode(observations)
        self.assertEqual(len(observations), len(path))
        for state in path:
            self.assertTrue(state in STATES)

    def test_dizzy_all_week_means_fever(self):
        probability, path = decode(["dizzy"] * 5)
        self.assertEqual(["fever"] * 5, path)


if __name__ == "__main__":
    unittest.main()
