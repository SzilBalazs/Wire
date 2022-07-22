#ifndef CHESS_MOVE_ORDERING_H
#define CHESS_MOVE_ORDERING_H

#include "move.h"

void clearKillerMoves();

void recordKillerMove(move, int);

void orderMoves(move *moves, unsigned int moveCount, int ply);

#endif //CHESS_MOVE_ORDERING_H
