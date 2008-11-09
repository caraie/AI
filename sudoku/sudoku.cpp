#include "sudoku.h"

#include <cstddef>

Sudoku::Sudoku()
{
    for (int i = 0; i < 81; ++i)
        m_cells[i] = 0;
}

bool Sudoku::load(const std::string &text)
{
    if (text.size() != 81) return false;

    for (int i = 0; i < 81; ++i)
    {
        char c = text[i];
        if (c == '.' || c == '0')
            m_cells[i] = 0;
        else if (c >= '1' && c <= '9')
            m_cells[i] = c - '0';
        else
            return false;
    }

    return true;
}

bool Sudoku::canPlace(int row, int column, int value) const
{
    for (int i = 0; i < 9; ++i)
    {
        if (i != column && at(row, i) == value) return false;
        if (i != row && at(i, column) == value) return false;
    }

    int boxRow = (row / 3) * 3;
    int boxColumn = (column / 3) * 3;

    for (int r = boxRow; r < boxRow + 3; ++r)
        for (int c = boxColumn; c < boxColumn + 3; ++c)
            if ((r != row || c != column) && at(r, c) == value)
                return false;

    return true;
}

int Sudoku::candidates(int row, int column) const
{
    int count = 0;
    for (int value = 1; value <= 9; ++value)
        if (canPlace(row, column, value))
            ++count;
    return count;
}

/* Most constrained empty cell first. Returns false when the grid is full. */
bool Sudoku::findBestCell(int &row, int &column) const
{
    int fewest = 10;
    bool found = false;

    for (int r = 0; r < 9; ++r)
    {
        for (int c = 0; c < 9; ++c)
        {
            if (at(r, c) != 0) continue;

            int count = candidates(r, c);
            if (count < fewest)
            {
                fewest = count;
                row = r;
                column = c;
                found = true;

                /* nothing beats a cell with a single candidate, and a cell
                   with none means this branch is already dead */
                if (count <= 1) return true;
            }
        }
    }

    return found;
}

bool Sudoku::solve()
{
    /* A grid that already breaks a rule can never be finished, and the
       search would happily spend an afternoon proving it. */
    if (!isValid()) return false;
    return search();
}

bool Sudoku::search()
{
    int row = 0;
    int column = 0;

    if (!findBestCell(row, column)) return true;

    for (int value = 1; value <= 9; ++value)
    {
        if (!canPlace(row, column, value)) continue;

        set(row, column, value);
        if (search()) return true;
        set(row, column, 0);
    }

    return false;
}

bool Sudoku::isComplete() const
{
    for (int i = 0; i < 81; ++i)
        if (m_cells[i] == 0)
            return false;
    return true;
}

bool Sudoku::isValid() const
{
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            if (at(r, c) != 0 && !canPlace(r, c, at(r, c)))
                return false;
    return true;
}

int Sudoku::givens() const
{
    int count = 0;
    for (int i = 0; i < 81; ++i)
        if (m_cells[i] != 0)
            ++count;
    return count;
}

std::string Sudoku::toString() const
{
    std::string text;
    for (int i = 0; i < 81; ++i)
        text += (char)(m_cells[i] == 0 ? '.' : '0' + m_cells[i]);
    return text;
}
