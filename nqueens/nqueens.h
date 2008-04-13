#ifndef NQUEENS_H
#define NQUEENS_H

#include <vector>

/*
 * N queens by backtracking. A board is a vector of n columns, position i
 * holds the row of the queen in column i, so two queens can never share a
 * column by construction.
 */
class NQueens
{
public:
    NQueens(int size);

    /* First solution found, empty if there is none. */
    std::vector<int> solve();

    /* Number of distinct solutions. */
    int countSolutions();

    static bool isValid(const std::vector<int> &board);

private:
    bool place(int column, bool stopAtFirst);

    int m_size;
    std::vector<int> m_board;
    std::vector<bool> m_rowUsed;
    std::vector<bool> m_risingUsed;   /* row + column */
    std::vector<bool> m_fallingUsed;  /* row - column, shifted to stay positive */
    int m_count;
};

#endif
