#ifndef CHESS_MOVE_ORDERING_H
#define CHESS_MOVE_ORDERING_H

#include "move.h"

const int mmvlva[6][6] = {
        {8004, 8304, 8104, 8204, 8404, 0}, // PAWN
        {8001, 8301, 8101, 8201, 8401, 0}, // ROOK
        {8003, 8303, 8103, 8203, 8403, 0}, // KNIGHT
        {8002, 8302, 8102, 8202, 8402, 0}, // BISHOP
        {8000, 8300, 8100, 8200, 8400, 0}, // QUEEN
        {0, 0, 0, 0, 0, 0}, // KING
};

void clearKillerMoves();

void recordKillerMove(move, int);

void orderMoves(move *moves, unsigned int moveCount, int ply);

#endif //CHESS_MOVE_ORDERING_H
