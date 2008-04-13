#include <cassert>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "nqueens.h"

namespace {

/* Known counts for n = 1..10, the small board with no solution included. */
const int EXPECTED[] = {1, 0, 0, 2, 10, 4, 40, 92, 352, 724};

void testCounts()
{
    for (int n = 1; n <= 10; ++n)
    {
        NQueens puzzle(n);
        assert(puzzle.countSolutions() == EXPECTED[n - 1]);
    }
}

void testFirstSolutionIsValid()
{
    for (int n = 4; n <= 9; ++n)
    {
        NQueens puzzle(n);
        std::vector<int> board = puzzle.solve();
        assert((int)board.size() == n);
        assert(NQueens::isValid(board));
    }
}

void testNoSolutionForThree()
{
    NQueens puzzle(3);
    assert(puzzle.solve().empty());
}

void testValidator()
{
    std::vector<int> good;
    good.push_back(1); good.push_back(3); good.push_back(0); good.push_back(2);
    assert(NQueens::isValid(good));

    std::vector<int> sameRow;
    sameRow.push_back(0); sameRow.push_back(0);
    assert(!NQueens::isValid(sameRow));

    std::vector<int> diagonal;
    diagonal.push_back(0); diagonal.push_back(1);
    assert(!NQueens::isValid(diagonal));
}

void printBoard(const std::vector<int> &board)
{
    for (std::size_t row = 0; row < board.size(); ++row)
    {
        for (std::size_t column = 0; column < board.size(); ++column)
            printf("%c", board[column] == (int)row ? 'Q' : '.');
        printf("\n");
    }
}

}

int main()
{
    testValidator();
    testNoSolutionForThree();
    testFirstSolutionIsValid();
    testCounts();

    NQueens puzzle(8);
    std::vector<int> board = puzzle.solve();
    printBoard(board);
    printf("8 queens: %d solutions\n", NQueens(8).countSolutions());
    printf("all tests passed\n");
    return 0;
}
