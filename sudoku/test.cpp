#include <cassert>
#include <cstdio>
#include <string>

#include "sudoku.h"

namespace {

/* A newspaper level puzzle. */
const char *EASY =
    "53..7...."
    "6..195..."
    ".98....6."
    "8...6...3"
    "4..8.3..1"
    "7...2...6"
    ".6....28."
    "...419..5"
    "....8..79";

/* The "world's hardest sudoku" that went round the papers, only 21 given
   numbers and plenty of backtracking. */
const char *HARD =
    "8........"
    "..36....."
    ".7..9.2.."
    ".5...7..."
    "....457.."
    "...1...3."
    "..1....68"
    "..85...1."
    ".9....4..";

/* Two nines in the first row, no solution possible. */
const char *BROKEN =
    "9.......9"
    "........."
    "........."
    "........."
    "........."
    "........."
    "........."
    "........."
    ".........";

void testLoad()
{
    Sudoku puzzle;
    assert(puzzle.load(EASY));
    assert(puzzle.at(0, 0) == 5);
    assert(puzzle.at(0, 2) == 0);
    assert(puzzle.givens() == 30);

    Sudoku bad;
    assert(!bad.load("too short"));
    assert(!bad.load(std::string(81, 'x')));
}

void testValidity()
{
    Sudoku puzzle;
    puzzle.load(EASY);
    assert(puzzle.isValid());
    assert(!puzzle.isComplete());

    Sudoku broken;
    broken.load(BROKEN);
    assert(!broken.isValid());
}

void testCanPlace()
{
    Sudoku puzzle;
    puzzle.load(EASY);

    assert(!puzzle.canPlace(0, 2, 5));  /* 5 already in the row */
    assert(!puzzle.canPlace(0, 2, 6));  /* 6 already in the box */
    assert(puzzle.canPlace(0, 2, 4));
}

void checkSolved(const Sudoku &puzzle)
{
    assert(puzzle.isComplete());
    assert(puzzle.isValid());

    for (int r = 0; r < 9; ++r)
    {
        int rowSum = 0;
        int columnSum = 0;
        for (int c = 0; c < 9; ++c)
        {
            rowSum += puzzle.at(r, c);
            columnSum += puzzle.at(c, r);
        }
        assert(rowSum == 45);
        assert(columnSum == 45);
    }
}

void testSolvesEasy()
{
    Sudoku puzzle;
    puzzle.load(EASY);
    assert(puzzle.solve());
    checkSolved(puzzle);

    /* the given numbers must still be where they were */
    Sudoku original;
    original.load(EASY);
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            if (original.at(r, c) != 0)
                assert(original.at(r, c) == puzzle.at(r, c));
}

void testSolvesHard()
{
    Sudoku puzzle;
    puzzle.load(HARD);
    assert(puzzle.givens() == 21);
    assert(puzzle.solve());
    checkSolved(puzzle);
}

void testRejectsBrokenGrid()
{
    Sudoku puzzle;
    puzzle.load(BROKEN);
    assert(!puzzle.solve());
}

void print(const Sudoku &puzzle)
{
    for (int r = 0; r < 9; ++r)
    {
        for (int c = 0; c < 9; ++c)
            printf("%d%s", puzzle.at(r, c), (c % 3 == 2 && c < 8) ? " | " : " ");
        printf("\n");
        if (r % 3 == 2 && r < 8) printf("------+-------+------\n");
    }
}

}

int main()
{
    testLoad();
    testValidity();
    testCanPlace();
    testSolvesEasy();
    testSolvesHard();
    testRejectsBrokenGrid();

    Sudoku puzzle;
    puzzle.load(HARD);
    puzzle.solve();
    print(puzzle);
    printf("all tests passed\n");
    return 0;
}
