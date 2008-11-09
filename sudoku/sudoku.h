#ifndef SUDOKU_H
#define SUDOKU_H

#include <string>

/*
 * Sudoku solver. Plain backtracking, but instead of walking the cells in
 * order it always fills the one with the fewest candidates left. On the
 * hard puzzles that is the difference between a blink and a coffee.
 */
class Sudoku
{
public:
    Sudoku();

    /* 81 characters, digits 1..9 and '.' or '0' for an empty cell. */
    bool load(const std::string &text);

    bool solve();

    int at(int row, int column) const { return m_cells[row * 9 + column]; }
    void set(int row, int column, int value) { m_cells[row * 9 + column] = value; }

    bool isComplete() const;
    bool isValid() const;
    bool canPlace(int row, int column, int value) const;
    int givens() const;

    std::string toString() const;

private:
    bool search();
    bool findBestCell(int &row, int &column) const;
    int candidates(int row, int column) const;

    int m_cells[81];
};

#endif
