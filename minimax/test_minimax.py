#!/usr/bin/env python
"""Tests for the tic tac toe engine."""

import unittest

import tictactoe
from tictactoe import X, O, EMPTY


def board_from(text):
    """Build a board from a nine character string, dots are empty cells."""
    return [EMPTY if c == "." else c for c in text]


class WinnerTest(unittest.TestCase):

    def test_empty_board_has_no_winner(self):
        self.assertEqual(None, tictactoe.winner(tictactoe.new_board()))

    def test_row(self):
        self.assertEqual(X, tictactoe.winner(board_from("XXX.O..O.")))

    def test_column(self):
        self.assertEqual(O, tictactoe.winner(board_from("O.XO.XO..")))

    def test_diagonal(self):
        self.assertEqual(X, tictactoe.winner(board_from("X.OOX..OX")))


class MinimaxTest(unittest.TestCase):

    def test_takes_the_win(self):
        board = board_from("XX.OO....")
        self.assertEqual(2, tictactoe.best_move(board, X))

    def test_blocks_the_other_side(self):
        # X threatens the bottom row, O cannot do anything but block
        board = board_from("O..O..X.X")
        self.assertEqual(7, tictactoe.best_move(board, O))

    def test_prefers_the_shorter_win(self):
        board = board_from("XX.OO..X.")
        self.assertEqual(2, tictactoe.best_move(board, X))

    def test_perfect_play_is_a_draw(self):
        board = tictactoe.new_board()
        turn = X
        while tictactoe.winner(board) is None and tictactoe.legal_moves(board):
            board[tictactoe.best_move(board, turn)] = turn
            turn = tictactoe.opponent(turn)
        self.assertEqual(None, tictactoe.winner(board))


if __name__ == "__main__":
    unittest.main()
