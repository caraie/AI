#!/usr/bin/env python
"""Tests for the knapsack genetic algorithm."""

import random
import unittest

import knapsack


SMALL = [("a", 5, 10), ("b", 4, 40), ("c", 6, 30), ("d", 3, 50)]
SMALL_CAPACITY = 10


def brute_force(items, capacity):
    """Best value by trying the 2^n subsets, only usable for tiny sets."""
    best = 0
    for mask in range(2 ** len(items)):
        chromosome = [(mask >> i) & 1 for i in range(len(items))]
        best = max(best, knapsack.fitness(chromosome, items, capacity))
    return best


class FitnessTest(unittest.TestCase):

    def test_empty_bag_is_worth_nothing(self):
        self.assertEqual(0, knapsack.fitness([0] * len(knapsack.ITEMS)))

    def test_overweight_bag_scores_zero(self):
        self.assertEqual(0, knapsack.fitness([1] * len(knapsack.ITEMS)))

    def test_value_and_weight_add_up(self):
        chromosome = [1, 0, 0, 1]
        self.assertEqual(8, knapsack.weight_of(chromosome, SMALL))
        self.assertEqual(60, knapsack.value_of(chromosome, SMALL))


class OperatorTest(unittest.TestCase):

    def test_crossover_keeps_the_length(self):
        rng = random.Random(1)
        a, b = knapsack.crossover([0, 0, 0, 0], [1, 1, 1, 1], rng)
        self.assertEqual(4, len(a))
        self.assertEqual(4, len(b))
        self.assertEqual(4, a.count(0) + a.count(1))

    def test_crossover_halves_come_from_the_parents(self):
        rng = random.Random(5)
        a, b = knapsack.crossover([0, 0, 0, 0], [1, 1, 1, 1], rng)
        # every gene of the first child is also in the same slot of a parent
        for i in range(4):
            self.assertTrue(a[i] in (0, 1))
            self.assertEqual(1, a[i] + b[i])

    def test_mutation_flips_everything_at_rate_one(self):
        chromosome = [0, 1, 0, 1]
        knapsack.mutate(chromosome, random.Random(1), 1.0)
        self.assertEqual([1, 0, 1, 0], chromosome)


class EvolveTest(unittest.TestCase):

    def test_finds_the_optimum_of_a_small_bag(self):
        # fixed seed, otherwise the run now and then settles for the second
        # best packing and the test starts blinking
        rng = random.Random(3)
        best = knapsack.evolve(SMALL, SMALL_CAPACITY, 20, 40, rng)
        self.assertEqual(brute_force(SMALL, SMALL_CAPACITY),
                         knapsack.fitness(best, SMALL, SMALL_CAPACITY))

    def test_result_fits_in_the_bag(self):
        rng = random.Random(7)
        best = knapsack.evolve(rng=rng)
        self.assertTrue(knapsack.weight_of(best) <= knapsack.CAPACITY)

    def test_gets_close_on_the_big_bag(self):
        # 715 is the optimum for this list at a capacity of 200
        rng = random.Random(2008)
        best = knapsack.evolve(rng=rng)
        self.assertTrue(knapsack.value_of(best) >= 700)


if __name__ == "__main__":
    unittest.main()
