#!/usr/bin/env python
"""Q learning on a small grid world.

The agent starts at S, gets +1 for reaching G, -1 for falling in the
pit and a small negative reward on every other step so that wandering
around is not free.
"""

import random

ACTIONS = ["up", "down", "left", "right"]

STEPS = {"up": (0, -1), "down": (0, 1), "left": (-1, 0), "right": (1, 0)}

WORLD = [
    "....G",
    ".##.#",
    ".#..P",
    "S....",
]


class GridWorld:

    def __init__(self, rows=None):
        self.rows = rows or WORLD
        self.height = len(self.rows)
        self.width = len(self.rows[0])

    def cell(self, position):
        x, y = position
        return self.rows[y][x]

    def start(self):
        return self.find("S")

    def find(self, mark):
        for y in range(self.height):
            for x in range(self.width):
                if self.rows[y][x] == mark:
                    return (x, y)
        return None

    def is_final(self, position):
        return self.cell(position) in "GP"

    def step(self, position, action):
        """Return (new position, reward). Walking into a wall wastes a turn."""
        dx, dy = STEPS[action]
        x, y = position[0] + dx, position[1] + dy

        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return position, -0.05
        if self.rows[y][x] == "#":
            return position, -0.05

        cell = self.rows[y][x]
        if cell == "G":
            return (x, y), 1.0
        if cell == "P":
            return (x, y), -1.0
        return (x, y), -0.05


def train(world, episodes=500, rate=0.5, discount=0.9, epsilon=0.2,
          limit=100, rng=None):
    """Run epsilon greedy Q learning and return the table."""
    if rng is None:
        rng = random.Random()

    q = {}
    for y in range(world.height):
        for x in range(world.width):
            for action in ACTIONS:
                q[((x, y), action)] = 0.0

    for episode in range(episodes):
        position = world.start()

        for step in range(limit):
            if rng.random() < epsilon:
                action = rng.choice(ACTIONS)
            else:
                action = best_action(q, position)

            new_position, reward = world.step(position, action)

            if world.is_final(new_position):
                future = 0.0
            else:
                future = q[(new_position, best_action(q, new_position))]

            old = q[(position, action)]
            q[(position, action)] = old + rate * (reward + discount * future - old)

            position = new_position
            if world.is_final(position):
                break

    return q


def best_action(q, position):
    best = ACTIONS[0]
    for action in ACTIONS[1:]:
        if q[(position, action)] > q[(position, best)]:
            best = action
    return best


def walk(world, q, limit=50):
    """Follow the greedy policy from the start and return the cells visited."""
    position = world.start()
    path = [position]

    for step in range(limit):
        if world.is_final(position):
            break
        position = world.step(position, best_action(q, position))[0]
        if position == path[-1]:
            break  # stuck against a wall, the policy is no good
        path.append(position)

    return path


if __name__ == "__main__":
    world = GridWorld()
    q = train(world, rng=random.Random(17))
    path = walk(world, q)

    marked = [list(row) for row in WORLD]
    for x, y in path:
        if marked[y][x] == ".":
            marked[y][x] = "o"
    for row in marked:
        print("".join(row))
    print("%d steps to %s" % (len(path) - 1, world.cell(path[-1])))
