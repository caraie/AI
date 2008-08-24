#!/usr/bin/env python
"""Tests for the markov chain."""

import random
import unittest

import chain
from chain import Chain


class TableTest(unittest.TestCase):

    def test_prefix_collects_every_continuation(self):
        c = Chain(2)
        c.add("a b c")
        c.add("a b d")
        self.assertEqual(["c", "d"], c.table[("a", "b")][:2])

    def test_short_lines_are_ignored(self):
        c = Chain(3)
        c.add("only two")
        self.assertEqual({}, c.table)
        self.assertEqual([], c.starts)

    def test_end_of_line_is_marked(self):
        c = Chain(1)
        c.add("a b")
        self.assertTrue(None in c.table[("b",)])


class GenerateTest(unittest.TestCase):

    def setUp(self):
        self.chain = Chain(2)
        for line in chain.SOURCE.strip().split("\n"):
            self.chain.add(line)

    def test_empty_chain_gives_empty_text(self):
        self.assertEqual("", Chain(2).generate())

    def test_same_seed_gives_the_same_text(self):
        first = self.chain.generate(20, random.Random(7))
        second = self.chain.generate(20, random.Random(7))
        self.assertEqual(first, second)

    def test_respects_the_limit(self):
        text = self.chain.generate(6, random.Random(3))
        self.assertTrue(len(text.split()) <= 6)

    def test_every_pair_comes_from_the_source(self):
        words = self.chain.generate(30, random.Random(11)).split()
        for i in range(len(words) - 2):
            prefix = (words[i], words[i + 1])
            self.assertTrue(words[i + 2] in self.chain.table[prefix])


if __name__ == "__main__":
    unittest.main()
