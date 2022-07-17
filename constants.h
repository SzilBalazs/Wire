#ifndef CHESS_CONSTANTS_H
#define CHESS_CONSTANTS_H

#include "piece.h"
#include <string>

#define U64 unsigned long long

#define fileA 0x101010101010101
#define fileH 0x8080808080808080
#define notFileA 0xfefefefefefefefeULL
#define notFileB 0xfdfdfdfdfdfdfdfdULL
#define notFileG 0xbfbfbfbfbfbfbfbfULL
#define notFileH 0x7f7f7f7f7f7f7f7fULL
#define rank1 0xffULL
#define rank2 0xff00ULL
#define notRank2 0xffffffffffff00ffULL
#define rank3 0xff0000ULL
#define rank4 0xff000000ULL
#define rank5 0xff00000000ULL
#define rank6 0xff0000000000ULL
#define rank7 0xff000000000000ULL
#define notRank7 0xff00ffffffffffffULL
#define rank8 0xff00000000000000ULL

const std::string ENGINE_NAME = "Wire";
const std::string AUTHOR = "SzilBalazs";
const std::string STARTING_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const int index64[64] = {
        0, 1, 48, 2, 57, 49, 28, 3,
        61, 58, 50, 42, 38, 29, 17, 4,
        62, 55, 59, 36, 53, 51, 43, 22,
        45, 39, 33, 30, 24, 18, 12, 5,
        63, 47, 56, 27, 60, 41, 37, 16,
        54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10,
        25, 14, 19, 9, 13, 8, 7, 6
};

const int centerDist[64] = {
        3, 3, 3, 3, 3, 3, 3, 3,
        3, 2, 2, 2, 2, 2, 2, 3,
        3, 2, 1, 1, 1, 1, 2, 3,
        3, 2, 1, 0, 0, 1, 2, 3,
        3, 2, 1, 0, 0, 1, 2, 3,
        3, 2, 1, 1, 1, 1, 2, 3,
        3, 2, 2, 2, 2, 2, 2, 3,
        3, 3, 3, 3, 3, 3, 3, 3
};

// castle right masks
const unsigned char WK_MASK = 1;
const unsigned char WQ_MASK = 2;
const unsigned char BK_MASK = 4;
const unsigned char BQ_MASK = 8;


// for finding piece by "mask" quickly
const Piece PIECE_TABLE[8] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, PIECE_EMPTY, PIECE_EMPTY};

enum Direction {
    NORTH = 0, WEST = 1, SOUTH = 2, EAST = 3, NoEa = 4, NoWe = 5, SoWe = 6, SoEa = 7
};

enum GameStatus {
    PLAYING = 0, CHECKMATE = 1, DRAW = 2
};

enum GameStage {
    OPENING = 0, MIDGAME = 1, ENDGAME = 2
};

const int INF = 1000000;

const int UNKNOWN_EVAL = 1111111;

const int MATE_SCORE = 10000;

#endif //CHESS_CONSTANTS_H
