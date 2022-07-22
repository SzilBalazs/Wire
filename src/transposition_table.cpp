#include "transposition_table.h"

HashRecord tt[SIZE];

int probeHash(unsigned int depth, int alpha, int beta) {
    U64 hash = b.getHash();
    HashRecord *record = &tt[hash & MASK];
    if (record->hash == hash) {
        if (record->depth >= depth) {
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
    if ((record->eval >= -MATE_SCORE+100 && record->eval <= MATE_SCORE-100) && (record->hash != hash || flag == EXACT || depth > (record->depth * 2) / 3)) {
        record->hash = hash;
        record->depth = depth;
        record->bestMove = bestMove;
        record->eval = score;
        record->flag = flag;
    }
}

move getBestMove() {
    U64 hash = b.getHash();
    HashRecord *record = &tt[hash & MASK];
    if (record->hash == hash) {
        return record->bestMove;
    }
    return {};
}
