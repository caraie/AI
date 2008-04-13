#include "nqueens.h"

#include <cstddef>

NQueens::NQueens(int size)
    : m_size(size),
      m_board(size, -1),
      m_rowUsed(size, false),
      m_risingUsed(2 * size, false),
      m_fallingUsed(2 * size, false),
      m_count(0)
{
}

/*
 * Instead of checking the new queen against every queen already on the
 * board, we keep one flag per row and per diagonal. Placing and undoing a
 * queen is then constant time.
 */
bool NQueens::place(int column, bool stopAtFirst)
{
    if (column == m_size)
    {
        ++m_count;
        return true;
    }

    bool found = false;

    for (int row = 0; row < m_size; ++row)
    {
        int rising = row + column;
        int falling = row - column + m_size;

        if (m_rowUsed[row] || m_risingUsed[rising] || m_fallingUsed[falling])
            continue;

        m_board[column] = row;
        m_rowUsed[row] = true;
        m_risingUsed[rising] = true;
        m_fallingUsed[falling] = true;

        if (place(column + 1, stopAtFirst))
        {
            found = true;
            if (stopAtFirst) return true;
        }

        m_rowUsed[row] = false;
        m_risingUsed[rising] = false;
        m_fallingUsed[falling] = false;
        m_board[column] = -1;
    }

    return found;
}

std::vector<int> NQueens::solve()
{
    m_count = 0;
    if (!place(0, true)) return std::vector<int>();
    return m_board;
}

int NQueens::countSolutions()
{
    m_count = 0;
    place(0, false);
    return m_count;
}

bool NQueens::isValid(const std::vector<int> &board)
{
    for (std::size_t i = 0; i < board.size(); ++i)
    {
        for (std::size_t j = i + 1; j < board.size(); ++j)
        {
            if (board[i] == board[j]) return false;

            int rowDistance = board[i] - board[j];
            if (rowDistance < 0) rowDistance = -rowDistance;
            if (rowDistance == (int)(j - i)) return false;
        }
    }
    return true;
}
