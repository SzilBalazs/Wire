#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

// encoding a piece:
// - first bit: color (this is why BLACK=8)
// - 2-4 bits: piece flag

enum Color : unsigned char {
    WHITE = 0, BLACK = 1, COLOR_EMPTY = 2
};
enum Piece : unsigned char {
    PAWN = 0, ROOK = 1, KNIGHT = 2, BISHOP = 3, QUEEN = 4, KING = 5, PIECE_EMPTY = 6
};

#endif //CHESS_PIECE_H
