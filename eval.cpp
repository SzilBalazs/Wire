#include "eval.h"
#include "globals.h"


int wPawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      5, 10, 10, -30, -30, 10, 10, 5,
                      10, -5, -5, 0, 0, -5, -5, 10,
                      0, 0, 0, 30, 30, 0, 0, 0,
                      5, 5, 5, 20, 20, 5, 5, 5,
                      10, 10, 20, 20, 20, 20, 10, 10,
                      50, 50, 50, 50, 50, 50, 50, 50,
                      0, 0, 0, 0, 0, 0, 0, 0};

int bPawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      50, 50, 50, 50, 50, 50, 50, 50,
                      10, 10, 20, 20, 20, 20, 10, 10,
                      5, 5, 5, 20, 20, 5, 5, 5,
                      0, 0, 0, 30, 30, 0, 0, 0,
                      10, -5, -5, 0, 0, -5, -5, 10,
                      5, 10, 10, -30, -30, 10, 10, 5,
                      0, 0, 0, 0, 0, 0, 0, 0};

int knightTable[64] = {-50, -40, -30, -30, -30, -30, -40, -50,
                       -40, -20, 0, 0, 0, 0, -20, -40,
                       -30, 0, 10, 15, 15, 10, 0, -30,
                       -30, 0, 15, 20, 20, 15, 0, -30,
                       -30, 0, 15, 20, 20, 15, 0, -30,
                       -30, 0, 10, 15, 15, 10, 0, -30,
                       -40, -20, 0, 0, 0, 0, -20, -40,
                       -50, -40, -30, -30, -30, -30, -40, -50};

int wBishopTable[64] = {-20, -10, -10, -10, -10, -10, -10, -20,
                        -5, 5, 0, 7, 7, 0, 5, -5,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -5, 0, 15, 10, 10, 15, 0, -5,
                        -5, 15, 0, 5, 5, 0, 15, -5,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -20, -10, -10, -10, -10, -10, -10, -20};

int bBishopTable[64] = {-20, -10, -10, -10, -10, -10, -10, -20,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -5, 15, 0, 5, 5, 0, 15, -5,
                        -5, 0, 15, 10, 10, 15, 0, -5,
                        -5, 0, 0, 0, 0, 0, 0, -5,
                        -5, 5, 0, 7, 7, 0, 5, -5,
                        -20, -10, -10, -10, -10, -10, -10, -20};

int bRookTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      5, 15, 15, 15, 15, 15, 15, 5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 5, 5, 5, 5, 0, -5};

int wRookTable[64] = {-5, 0, 5, 5, 5, 5, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      5, 15, 15, 15, 15, 15, 15, 5,
                      0, 0, 0, 0, 0, 0, 0, 0};

int wKingTable[64] = {30, 30, 10, 0, 0, 10, 30, 30,
                      20, 20, 0, 0, 0, 0, 20, 20,
                      -10, -20, -20, -20, -20, -20, -20, -10,
                      -20, -25, -30, -40, -40, -30, -25, -20,
                      -30, -40, -40, -40, -40, -40, -40, -30,
                      -40, -40, -40, -40, -40, -40, -40, -40,
                      -40, -50, -50, -50, -50, -50, -50, -40,
                      -60, -60, -60, -60, -60, -60, -60, -60,};

int bKingTable[64] = {-60, -60, -60, -60, -60, -60, -60, -60,
                      -40, -50, -50, -50, -50, -50, -50, -40,
                      -40, -40, -40, -40, -40, -40, -40, -40,
                      -30, -40, -40, -40, -40, -40, -40, -30,
                      -20, -25, -30, -40, -40, -30, -25, -20,
                      -10, -20, -20, -20, -20, -20, -20, -10,
                      20, 20, 0, 0, 0, 0, 20, 20,
                      30, 30, 10, 0, 0, 10, 30, 30};

int basePieceValue[6] = {90, 500, 200, 300, 900};

int eval() {
    GameStage stage = OPENING; // <- OPENING is not used
    int pawnCount = b.pieceBB[PAWN].pop_cnt();
    /*int queenCount = b.pieceBB[QUEEN].pop_cnt();
    int minorCount = (b.pieceBB[KNIGHT] & b.pieceBB[BISHOP]).pop_cnt();
    int majorCount = b.pieceBB[ROOK].pop_cnt();
    if ((queenCount <= 2 && minorCount <= 2 && majorCount == 0) ||
        (queenCount <= 1 && minorCount <= 4 && majorCount <= 2)) {
        stage = ENDGAME;
    } else {
        stage = MIDGAME;
    }*/

    // white eval
    int whiteEval = 0;
    bitboard wPawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];
    bitboard wRooks = b.allPieceBB[WHITE] & b.pieceBB[ROOK];
    bitboard wKnights = b.allPieceBB[WHITE] & b.pieceBB[KNIGHT];
    bitboard wBishops = b.allPieceBB[WHITE] & b.pieceBB[BISHOP];
    bitboard wQueens = b.allPieceBB[WHITE] & b.pieceBB[QUEEN];
    while (wPawns) {
        unsigned int index = wPawns.pop_lsb();
        whiteEval += basePieceValue[PAWN] + wPawnTable[index];
    }
    while (wRooks) {
        unsigned int index = wRooks.pop_lsb();
        whiteEval += basePieceValue[ROOK] + wRookTable[index];
    }
    while (wKnights) {
        unsigned int index = wKnights.pop_lsb();
        whiteEval += (basePieceValue[KNIGHT] + pawnCount * 5) + knightTable[index];
    }
    while (wBishops) {
        unsigned int index = wBishops.pop_lsb();
        whiteEval += basePieceValue[BISHOP] + wBishopTable[index];
    }
    while (wQueens) {
        unsigned int index = wQueens.pop_lsb();
        whiteEval += basePieceValue[QUEEN]; // TODO maybe add more stuff?
    }
    whiteEval += wKingTable[b.wking];

    // black eval
    int blackEval = 0;
    bitboard bPawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];
    bitboard bRooks = b.allPieceBB[BLACK] & b.pieceBB[ROOK];
    bitboard bKnights = b.allPieceBB[BLACK] & b.pieceBB[KNIGHT];
    bitboard bBishops = b.allPieceBB[BLACK] & b.pieceBB[BISHOP];
    bitboard bQueens = b.allPieceBB[BLACK] & b.pieceBB[QUEEN];

    while (bPawns) {
        unsigned int index = bPawns.pop_lsb();
        blackEval += basePieceValue[PAWN] + bPawnTable[index];
    }
    while (bRooks) {
        unsigned int index = bRooks.pop_lsb();
        blackEval += basePieceValue[ROOK] + bRookTable[index];
    }
    while (bKnights) {
        unsigned int index = bKnights.pop_lsb();
        blackEval += (basePieceValue[KNIGHT] + pawnCount * 5) + knightTable[index];
    }
    while (bBishops) {
        unsigned int index = bBishops.pop_lsb();
        blackEval += basePieceValue[BISHOP] + bBishopTable[index];
    }
    while (bQueens) {
        unsigned int index = bQueens.pop_lsb();
        blackEval += basePieceValue[QUEEN]; // TODO maybe add more stuff?
    }
    blackEval += bKingTable[b.bking];

    if (b.stm == WHITE)
        return whiteEval - blackEval;
    else
        return blackEval - whiteEval;
}
