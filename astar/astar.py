#!/usr/bin/env python
"""A* over a rectangular grid.

The map is a list of equally long strings, '#' is a wall and anything
else is walkable. Movement is four directional, so the Manhattan
distance is an admissible heuristic (it never overestimates).
"""

import heapq


class Grid:

    def __init__(self, rows):
        self.rows = rows
        self.height = len(rows)
        self.width = len(rows[0])

    def walkable(self, cell):
        x, y = cell
        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return False
        return self.rows[y][x] != "#"

    def neighbours(self, cell):
        x, y = cell
        candidates = [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]
        return [c for c in candidates if self.walkable(c)]


def manhattan(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


def find_path(grid, start, goal):
    """Return the list of cells from start to goal, or None if unreachable."""
    if not grid.walkable(start) or not grid.walkable(goal):
        return None

    # the heap holds (f, g, cell); f is what orders the queue and g is
    # kept around because we need it to expand the neighbours
    open_heap = [(manhattan(start, goal), 0, start)]
    came_from = {}
    best_cost = {start: 0}
    closed = {}

    while open_heap:
        f, cost, cell = heapq.heappop(open_heap)

        if cell in closed:
            # already expanded through a cheaper path, this is a leftover
            continue
        closed[cell] = True

        if cell == goal:
            return rebuild(came_from, cell)

        for step in grid.neighbours(cell):
            new_cost = cost + 1
            if step in best_cost and best_cost[step] <= new_cost:
                continue
            best_cost[step] = new_cost
            came_from[step] = cell
            heapq.heappush(open_heap, (new_cost + manhattan(step, goal), new_cost, step))

    return None


def rebuild(came_from, cell):
    path = [cell]
    while cell in came_from:
        cell = came_from[cell]
        path.append(cell)
    path.reverse()
    return path


MAP = [
    "..........",
    ".####.###.",
    ".#....#...",
    ".#.####.##",
    ".#........",
    ".#####.##.",
    "......#...",
]


if __name__ == "__main__":
    grid = Grid(MAP)
    path = find_path(grid, (0, 0), (9, 6))
    if path is None:
        print("no path")
    else:
        marked = [list(row) for row in MAP]
        for x, y in path:
            marked[y][x] = "o"
        for row in marked:
            print("".join(row))
        print("%d steps" % (len(path) - 1))
