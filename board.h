#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include "piece.h"
#include "utils.h"
#include "move.h"
#include "bitboard.h"

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

void hash_init();

class board {
private:
    U64 hash;

    std::vector<U64> hashHistory;

    std::stack<unsigned int> lastIrreversibleMove;

    void setSq(Color, Piece, unsigned int);

    void clearSq(unsigned int);

    void movePiece(unsigned int, unsigned int);

    void movePiece(unsigned int, unsigned int, Piece);

    void movePiece(unsigned int, unsigned int, Piece, Color);

    void updateHash();

public:
    // board representation
    Piece pieces[64];
    Color colors[64];
    bitboard pieceBB[7];
    bitboard allPieceBB[3];
    Color stm; // side to move 0: white, 1: black
    std::stack<move> moves;

    std::stack<int> ep; // en passan square

    std::stack<unsigned char> castle_rights;
    unsigned int wking, bking;
    // extra bitboards and calculation stuff
    GameStatus status;
    bitboard occupied, empty, kingDanger, checkers, captureMask, pushMask, pinned;
    bitboard pinMasks[64];

    unsigned int checks, kingPos;

    bool canCastle(unsigned char) const;

    void removeCastleRight(unsigned char);

    void clearPosition();

    void loadPositionFromFEN(std::string);

    void displayBoard();

    void makeMove(move);

    void undoMove();

    U64 getHash();
};

#endif //CHESS_BOARD_H
