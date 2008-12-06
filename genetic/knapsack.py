#!/usr/bin/env python
"""Genetic algorithm for the 0/1 knapsack problem.

A chromosome is a list of zeros and ones, one per item. Selection is by
tournament, which is easier to get right than roulette wheel and does
not care about the scale of the fitness.
"""

import random

# (name, weight, value)
ITEMS = [
    ("map", 9, 150), ("compass", 13, 35), ("water", 153, 200),
    ("sandwich", 50, 160), ("glucose", 15, 60), ("banana", 27, 60),
    ("suntan cream", 11, 70), ("camera", 32, 30), ("t-shirt", 24, 15),
    ("trousers", 48, 10), ("umbrella", 73, 40), ("note-case", 22, 80),
    ("sunglasses", 7, 20), ("towel", 18, 12), ("socks", 4, 50),
]

CAPACITY = 200


def weight_of(chromosome, items=ITEMS):
    total = 0
    for i in range(len(chromosome)):
        if chromosome[i]:
            total += items[i][1]
    return total


def value_of(chromosome, items=ITEMS):
    total = 0
    for i in range(len(chromosome)):
        if chromosome[i]:
            total += items[i][2]
    return total


def fitness(chromosome, items=ITEMS, capacity=CAPACITY):
    """Overweight bags are worth nothing, no partial credit."""
    if weight_of(chromosome, items) > capacity:
        return 0
    return value_of(chromosome, items)


def random_chromosome(size, rng):
    return [rng.randint(0, 1) for i in range(size)]


def tournament(population, scores, rng, size=3):
    best = rng.randrange(len(population))
    for i in range(size - 1):
        other = rng.randrange(len(population))
        if scores[other] > scores[best]:
            best = other
    return population[best]


def crossover(a, b, rng):
    cut = rng.randrange(1, len(a))
    return a[:cut] + b[cut:], b[:cut] + a[cut:]


def mutate(chromosome, rng, rate=0.02):
    for i in range(len(chromosome)):
        if rng.random() < rate:
            chromosome[i] = 1 - chromosome[i]


def evolve(items=ITEMS, capacity=CAPACITY, population_size=60,
           generations=120, rng=None):
    if rng is None:
        rng = random.Random()

    size = len(items)
    population = [random_chromosome(size, rng) for i in range(population_size)]
    best = population[0]

    for generation in range(generations):
        scores = [fitness(c, items, capacity) for c in population]

        for i in range(len(population)):
            if scores[i] > fitness(best, items, capacity):
                best = population[i][:]

        # keep the best one untouched, otherwise a good solution can be
        # lost to a bad crossover and never come back
        children = [best[:]]
        while len(children) < population_size:
            first, second = crossover(tournament(population, scores, rng),
                                      tournament(population, scores, rng), rng)
            mutate(first, rng)
            mutate(second, rng)
            children.append(first)
            if len(children) < population_size:
                children.append(second)

        population = children

    return best


if __name__ == "__main__":
    rng = random.Random(2008)
    best = evolve(rng=rng)
    print("value %d, weight %d of %d" % (value_of(best), weight_of(best), CAPACITY))
    for i in range(len(ITEMS)):
        if best[i]:
            print("  " + ITEMS[i][0])
