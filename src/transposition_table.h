#ifndef CHESS_TRANSPOSITION_TABLE_H
#define CHESS_TRANSPOSITION_TABLE_H

#include "constants.h"
#include "globals.h"
#include "move.h"


const unsigned int SIZE = (1UL << 24);
const unsigned int MASK = SIZE - 1;

enum EntryFlag : unsigned char {
    EXACT = 0, ALPHA = 1, BETA = 2
};

struct HashRecord {
    U64 hash;
    unsigned int depth;
    EntryFlag flag;
    int eval;
    move bestMove;
};

int probeHash(unsigned int depth, int alpha, int beta);

void saveHash(unsigned int depth, move bestMove, int score, EntryFlag flag);

move getBestMove();


#endif //CHESS_TRANSPOSITION_TABLE_H
