#!/usr/bin/env python
"""Tests for the A* implementation."""

import unittest

import astar


OPEN = ["....",
        "....",
        "...."]

WALLED = ["....",
          ".##.",
          "....",
          "####"]

SPLIT = ["..#..",
         "..#..",
         "..#.."]


class PathTest(unittest.TestCase):

    def test_straight_line(self):
        grid = astar.Grid(OPEN)
        path = astar.find_path(grid, (0, 0), (3, 0))
        self.assertEqual([(0, 0), (1, 0), (2, 0), (3, 0)], path)

    def test_same_cell(self):
        grid = astar.Grid(OPEN)
        self.assertEqual([(2, 1)], astar.find_path(grid, (2, 1), (2, 1)))

    def test_walks_around_the_wall(self):
        grid = astar.Grid(WALLED)
        path = astar.find_path(grid, (0, 0), (3, 2))
        # the shortest way round the block is five steps
        self.assertEqual(6, len(path))
        self.assertEqual((0, 0), path[0])
        self.assertEqual((3, 2), path[-1])

    def test_every_step_is_legal(self):
        grid = astar.Grid(WALLED)
        path = astar.find_path(grid, (0, 0), (3, 2))
        for cell in path:
            self.assertTrue(grid.walkable(cell))
        for i in range(len(path) - 1):
            self.assertEqual(1, astar.manhattan(path[i], path[i + 1]))

    def test_unreachable_goal(self):
        grid = astar.Grid(SPLIT)
        self.assertEqual(None, astar.find_path(grid, (0, 0), (4, 0)))

    def test_goal_inside_a_wall(self):
        grid = astar.Grid(SPLIT)
        self.assertEqual(None, astar.find_path(grid, (0, 0), (2, 1)))


if __name__ == "__main__":
    unittest.main()
