#!/usr/bin/env python
"""Tests for the grid world and the Q learning agent."""

import random
import unittest

import gridworld
from gridworld import GridWorld


class WorldTest(unittest.TestCase):

    def setUp(self):
        self.world = GridWorld()

    def test_finds_the_start(self):
        self.assertEqual((0, 3), self.world.start())

    def test_walking_into_a_wall_keeps_the_position(self):
        position, reward = self.world.step((1, 0), "down")
        self.assertEqual((1, 0), position)
        self.assertTrue(reward < 0)

    def test_edge_of_the_map_is_a_wall_too(self):
        self.assertEqual((0, 0), self.world.step((0, 0), "up")[0])

    def test_reaching_the_goal_pays(self):
        self.assertEqual(1.0, self.world.step((3, 0), "right")[1])

    def test_falling_in_the_pit_costs(self):
        self.assertEqual(-1.0, self.world.step((4, 1), "down")[1])

    def test_goal_and_pit_are_final(self):
        self.assertTrue(self.world.is_final((4, 0)))
        self.assertTrue(self.world.is_final((4, 2)))
        self.assertTrue(not self.world.is_final((0, 3)))


class LearningTest(unittest.TestCase):

    def setUp(self):
        self.world = GridWorld()
        self.q = gridworld.train(self.world, rng=random.Random(17))

    def test_policy_reaches_the_goal(self):
        path = gridworld.walk(self.world, self.q)
        self.assertEqual((4, 0), path[-1])

    def test_policy_takes_the_short_way(self):
        # seven moves is the shortest route from S to G on this map
        path = gridworld.walk(self.world, self.q)
        self.assertEqual(8, len(path))

    def test_the_pit_is_worth_less_than_the_goal(self):
        pit_side = self.q[((3, 2), "right")]
        goal_side = self.q[((3, 0), "right")]
        self.assertTrue(goal_side > pit_side)


if __name__ == "__main__":
    unittest.main()
