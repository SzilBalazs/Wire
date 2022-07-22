#include "eval.h"
#include "globals.h"
#include "movegen.h"


int wPawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      10, 10, 10, -30, -30, 10, 10, 10,
                      10, 10, -5, 0, 0, -5, 10, 10,
                      0, 0, 0, 30, 30, 0, 0, 0,
                      5, 5, 5, 20, 20, 5, 5, 5,
                      5, 5, 15, 15, 15, 15, 5, 5,
                      50, 50, 50, 50, 50, 50, 50, 50,
                      0, 0, 0, 0, 0, 0, 0, 0};

int bPawnTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      50, 50, 50, 50, 50, 50, 50, 50,
                      5, 5, 15, 15, 15, 15, 5, 5,
                      5, 5, 5, 20, 20, 5, 5, 5,
                      0, 0, 0, 30, 30, 0, 0, 0,
                      10, 10, -5, 0, 0, -5, 10, 10,
                      10, 10, 10, -30, -30, 10, 10, 10,
                      0, 0, 0, 0, 0, 0, 0, 0};

int knightTable[64] = {-50, -40, -30, -30, -30, -30, -40, -50,
                       -40, -20, 5, 5, 5, 5, -20, -40,
                       -30, 5, 10, 15, 15, 10, 5, -30,
                       -30, 5, 15, 20, 20, 15, 5, -30,
                       -30, 5, 15, 20, 20, 15, 5, -30,
                       -30, 5, 10, 15, 15, 10, 5, -30,
                       -40, -20, 5, 5, 5, 5, -20, -40,
                       -50, -40, -30, -30, -30, -30, -40, -50};

int wBishopTable[64] = {-20, -10, -15, -10, -10, -15, -10, -20,
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
                        -20, -10, -15, -10, -10, -15, -10, -20};

int bRookTable[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                      5, 15, 15, 15, 15, 15, 15, 5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5,
                      -5, 0, 0, 0, 0, 0, 0, -5};

int wRookTable[64] = {-5, 0, 0, 0, 0, 0, 0, -5,
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
                      -60, -60, -60, -60, -60, -60, -60, -60};

int bKingTable[64] = {-60, -60, -60, -60, -60, -60, -60, -60,
                      -40, -50, -50, -50, -50, -50, -50, -40,
                      -40, -40, -40, -40, -40, -40, -40, -40,
                      -30, -40, -40, -40, -40, -40, -40, -30,
                      -20, -25, -30, -40, -40, -30, -25, -20,
                      -10, -20, -20, -20, -20, -20, -20, -10,
                      20, 20, 0, 0, 0, 0, 20, 20,
                      30, 30, 10, 0, 0, 10, 30, 30};

int basePieceValue[6] = {100, 600, 250, 360, 1050};

const int TEMPO_SCORE = 10;

// pawn values
const int BLOCKED_PAWN_PENALTY = -10;
const int PASSED_PAWN_BONUS = 15;
const int CHAIN_PAWN_BONUS = 5;
const int DOUBLE_PAWN_PENALTY = -5;
const int ISOLATED_PAWN_PENALTY = -15;

// rook values
const int OPEN_FILE = 30;
const int HALF_OPEN_FILE = 15;

GameStage stage = OPENING; // <- OPENING is not used

int evalWhitePawns() {
    int score = 0;
    bitboard wPawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];
    bitboard bPawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];

    // Blocked Pawns
    bitboard blockedPawns = wPawns.step(NORTH) & b.allPieceBB[BLACK];
    score += BLOCKED_PAWN_PENALTY * blockedPawns.pop_cnt();

    bitboard _wPawns = wPawns;
    while (_wPawns) {
        unsigned int index = _wPawns.pop_lsb();

        score += basePieceValue[PAWN] + wPawnTable[index];

        // In endgame, we will reward the engine for making progress forward with the pawns
        if (stage == ENDGAME) {
            score += (indexToRank(index) - 2) * 5;
        }

        // Check for a chain pawn
        if (movegen::pawnAttacks(index, BLACK) & wPawns) {
            score += CHAIN_PAWN_BONUS;
        }

        // Check for double pawn
        // DOUBLE_PAWN_PENALTY is added to the score twice if double, thrice if triple and so on
        if (movegen::getFileMaskEx(index) & wPawns) {
            score += DOUBLE_PAWN_PENALTY;
        }

        // Check for isolated pawn
        bool foundSupportingPawn = false;
        if ((indexToFile(index) != 0) && (movegen::getFileMask(index - 1) & wPawns)) {
            foundSupportingPawn = true;
        }
        if ((indexToFile(index) != 7) && (movegen::getFileMask(index + 1) & wPawns)) {
            foundSupportingPawn = true;
        }
        if (!foundSupportingPawn) {
            score += ISOLATED_PAWN_PENALTY;
        }

        // Check for passed pawn
        bool foundEnemyPawn = false;
        if ((indexToFile(index) != 0) && (movegen::getRayMask(index - 1, NORTH) & bPawns)) {
            foundEnemyPawn = true;
        }
        if (movegen::getRayMask(index, NORTH) & bPawns) {
            foundEnemyPawn = true;
        }
        if ((indexToFile(index) != 7) && (movegen::getRayMask(index + 1, NORTH) & bPawns)) {
            foundEnemyPawn = true;
        }
        if (!foundEnemyPawn) {
            score += PASSED_PAWN_BONUS;
        }
    }
    return score;
}

int evalBlackPawns() {
    int score = 0;
    bitboard wPawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];
    bitboard bPawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];

    // Blocked Pawns
    bitboard blockedPawns = bPawns.step(SOUTH) & b.allPieceBB[WHITE];
    score += BLOCKED_PAWN_PENALTY * blockedPawns.pop_cnt();

    bitboard _bPawns = bPawns;
    while (_bPawns) {
        unsigned int index = _bPawns.pop_lsb();

        score += basePieceValue[PAWN] + bPawnTable[index];

        // In endgame, we will reward the engine for making progress forward with the pawns
        if (stage == ENDGAME) {
            score += (7 - indexToRank(index)) * 5;
        }

        // Check for a chain pawn
        if (movegen::pawnAttacks(index, WHITE) & bPawns) {
            score += CHAIN_PAWN_BONUS;
        }

        // Check for double pawn
        // DOUBLE_PAWN_PENALTY is added to the score twice if double, thrice if triple and so on
        if (movegen::getFileMaskEx(index) & bPawns) {
            score += DOUBLE_PAWN_PENALTY;
        }

        // Check for isolated pawn
        bool foundSupportingPawn = false;
        if ((indexToFile(index) != 0) && (movegen::getFileMask(index - 1) & bPawns)) {
            foundSupportingPawn = true;
        }
        if ((indexToFile(index) != 7) && (movegen::getFileMask(index + 1) & bPawns)) {
            foundSupportingPawn = true;
        }
        if (!foundSupportingPawn) {
            score += ISOLATED_PAWN_PENALTY;
        }

        // Check for passed pawn
        bool foundEnemyPawn = false;
        if ((indexToFile(index) != 0) && (movegen::getRayMask(index - 1, SOUTH) & wPawns)) {
            foundEnemyPawn = true;
        }
        if (movegen::getRayMask(index, SOUTH) & wPawns) {
            foundEnemyPawn = true;
        }
        if ((indexToFile(index) != 7) && (movegen::getRayMask(index + 1, SOUTH) & wPawns)) {
            foundEnemyPawn = true;
        }
        if (!foundEnemyPawn) {
            score += PASSED_PAWN_BONUS;
        }
    }
    return score;
}

int eval() {
    movegen::update_general_data();

    unsigned int endgameWeight = b.pieceBB[QUEEN].pop_cnt() * 3 + b.pieceBB[ROOK].pop_cnt() * 2 +
                                 (b.pieceBB[KNIGHT] | b.pieceBB[BISHOP]).pop_cnt();
    if (endgameWeight <= 8) {
        stage = ENDGAME;
    } else {
        stage = MIDGAME;
    }

    int pawnCount = b.pieceBB[PAWN].pop_cnt();

    // white eval
    int whiteEval = 0;

    bitboard wRooks = b.allPieceBB[WHITE] & b.pieceBB[ROOK];
    bitboard wKnights = b.allPieceBB[WHITE] & b.pieceBB[KNIGHT];
    bitboard wBishops = b.allPieceBB[WHITE] & b.pieceBB[BISHOP];
    bitboard wQueens = b.allPieceBB[WHITE] & b.pieceBB[QUEEN];

    whiteEval += evalWhitePawns();

    while (wRooks) {
        unsigned int index = wRooks.pop_lsb();
        whiteEval += (basePieceValue[ROOK] + (16 - pawnCount) * 5) + wRookTable[index];
        unsigned int filePawnCount = (movegen::getFileMask(index) & b.pieceBB[PAWN]).pop_cnt();
        if (filePawnCount == 0) whiteEval += OPEN_FILE;
        if (filePawnCount == 1) whiteEval += HALF_OPEN_FILE;
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

    if (stage == MIDGAME) {
        whiteEval += wKingTable[b.wking];
    } else if (stage == ENDGAME) {
        whiteEval += (2 - centerDist[b.wking]) * 20;
    }


    // black eval
    int blackEval = 0;

    bitboard bRooks = b.allPieceBB[BLACK] & b.pieceBB[ROOK];
    bitboard bKnights = b.allPieceBB[BLACK] & b.pieceBB[KNIGHT];
    bitboard bBishops = b.allPieceBB[BLACK] & b.pieceBB[BISHOP];
    bitboard bQueens = b.allPieceBB[BLACK] & b.pieceBB[QUEEN];

    blackEval += evalBlackPawns();

    while (bRooks) {
        unsigned int index = bRooks.pop_lsb();
        blackEval += (basePieceValue[ROOK] + (16 - pawnCount) * 5) + bRookTable[index];
        unsigned int filePawnCount = (movegen::getFileMask(index) & b.pieceBB[PAWN]).pop_cnt();
        if (filePawnCount == 0) blackEval += OPEN_FILE;
        if (filePawnCount == 1) blackEval += HALF_OPEN_FILE;
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

    if (stage == MIDGAME) {
        blackEval += bKingTable[b.bking];
    } else if (stage == ENDGAME) {
        blackEval += (2 - centerDist[b.bking]) * 20;
    }

    if (b.stm == WHITE)
        return whiteEval - blackEval + TEMPO_SCORE;
    else
        return blackEval - whiteEval + TEMPO_SCORE;
}
