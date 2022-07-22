//
// Created by user on 2022. 07. 11..
//

#include "transposition_table.h"

HashRecord tt[SIZE];

int probeHash(unsigned int depth, int alpha, int beta) {
    U64 hash = b.getHash();
    HashRecord *record = &tt[hash & MASK];
    if (record->hash == hash) {
        if (record->depth >= depth && (record->eval >= -MATE_SCORE+100 && record->eval <= MATE_SCORE-100)) { // we don't want to get mate scores because the depth is messed up
            // we already calculated an eval at a higher depth
            if (record->flag == EXACT) {
                return record->eval;
            }
            // last time we didn't found a better eval than alpha
            if ((record->flag == ALPHA) && (record->eval <= alpha)) {
                return alpha;
            }
            // last time we encountered a beta-cutoff
            if ((record->flag == BETA) && (record->eval >= beta)) {
                return beta;
            }
        }
    }
    return UNKNOWN_EVAL;
}

void saveHash(unsigned int depth, move bestMove, int score, EntryFlag flag) {
    U64 hash = b.getHash();
    HashRecord *record = &tt[hash & MASK];
    // TODO use a better replacement scheme
    record->hash = hash;
    record->depth = depth;
    record->bestMove = bestMove;
    record->eval = score;
    record->flag = flag;
}

move getBestMove() {
    U64 hash = b.getHash();
    HashRecord *record = &tt[hash & MASK];
    if (record->hash == hash) {
        return record->bestMove;
    }
    return {};
}
