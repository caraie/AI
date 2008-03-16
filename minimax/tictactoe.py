#!/usr/bin/env python
"""Minimax for tic tac toe.

Full depth search, the game tree is small enough to walk it whole.
A board is just a list of nine cells holding 'X', 'O' or a space.
"""

X = "X"
O = "O"
EMPTY = " "

# the eight ways of winning: rows, columns and the two diagonals
LINES = [(0, 1, 2), (3, 4, 5), (6, 7, 8),
         (0, 3, 6), (1, 4, 7), (2, 5, 8),
         (0, 4, 8), (2, 4, 6)]


def new_board():
    return [EMPTY] * 9


def winner(board):
    """Return 'X' or 'O' if somebody has a line, None otherwise."""
    for a, b, c in LINES:
        if board[a] != EMPTY and board[a] == board[b] == board[c]:
            return board[a]
    return None


def legal_moves(board):
    return [i for i in range(9) if board[i] == EMPTY]


def opponent(player):
    if player == X:
        return O
    return X


def score(board, player, depth):
    """Value of a finished board from player's point of view.

    The depth is subtracted from the score so a win in two moves looks
    better than the same win in four. Without this the engine finds a
    forced win and then wanders around instead of finishing.
    """
    won = winner(board)
    if won == player:
        return 10 - depth
    if won is not None:
        return depth - 10
    return 0


def minimax(board, player, turn, depth=0):
    """Return (value, move) for the side to move.

    player is the side we are scoring for, turn is the side that has to
    play now, so on odd plies we minimize instead of maximize.
    """
    if winner(board) is not None or not legal_moves(board):
        return score(board, player, depth), None

    maximizing = (turn == player)
    best_value = None
    best_move = None

    for move in legal_moves(board):
        board[move] = turn
        value = minimax(board, player, opponent(turn), depth + 1)[0]
        board[move] = EMPTY

        if best_value is None:
            better = True
        elif maximizing:
            better = value > best_value
        else:
            better = value < best_value

        if better:
            best_value = value
            best_move = move

    return best_value, best_move


def best_move(board, player):
    return minimax(board, player, player)[1]


def show(board):
    rows = []
    for i in range(0, 9, 3):
        rows.append("|".join(board[i:i + 3]))
    return "\n-+-+-\n".join(rows)


if __name__ == "__main__":
    # let the engine play itself, it should always end in a draw
    board = new_board()
    turn = X
    while winner(board) is None and legal_moves(board):
        board[best_move(board, turn)] = turn
        turn = opponent(turn)
    print(show(board))
    print("winner: %s" % (winner(board) or "draw"))
