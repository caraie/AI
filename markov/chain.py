#!/usr/bin/env python
"""Markov chain text generator.

Builds a table of prefix -> possible next words and then walks it at
random. Order 1 gives noise, order 2 already sounds like the source.
"""

import random


class Chain:

    def __init__(self, order=2):
        self.order = order
        self.table = {}
        self.starts = []

    def add(self, text):
        words = text.split()
        if len(words) <= self.order:
            return

        self.starts.append(tuple(words[:self.order]))

        for i in range(len(words) - self.order):
            prefix = tuple(words[i:i + self.order])
            self.table.setdefault(prefix, []).append(words[i + self.order])

        # the last prefix has no continuation, mark it as an ending
        self.table.setdefault(tuple(words[-self.order:]), []).append(None)

    def generate(self, limit=40, rng=None):
        if rng is None:
            rng = random.Random()
        if not self.starts:
            return ""

        prefix = rng.choice(self.starts)
        words = list(prefix)

        while len(words) < limit:
            choices = self.table.get(prefix)
            if not choices:
                break
            word = rng.choice(choices)
            if word is None:
                break
            words.append(word)
            prefix = tuple(words[-self.order:])

        return " ".join(words)


SOURCE = """
the quick brown fox jumps over the lazy dog while the lazy dog sleeps
the brown fox runs over the hill and the quick dog follows the fox
a lazy afternoon in the sun makes the dog sleep and the fox wait
"""


if __name__ == "__main__":
    chain = Chain(2)
    for line in SOURCE.strip().split("\n"):
        chain.add(line)

    rng = random.Random(1234)
    for i in range(5):
        print(chain.generate(20, rng))
