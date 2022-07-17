#include <vector>
#include "move.h"
#include "piece.h"
#include "board.h"
#include "constants.h"

#ifndef CHESS_MOVEGEN_H
#define CHESS_MOVEGEN_H

namespace movegen {

    bitboard getRayIntoSq(unsigned int, unsigned int);

    bitboard fileAttacks(unsigned int);

    bitboard rankAttacks(unsigned int);

    bitboard diagonalAttacks(unsigned int);

    bitboard antiDiagonalAttacks(unsigned int);

    bitboard rookAttacks(unsigned int);

    bitboard bishopAttacks(unsigned int);

    bitboard queenAttacks(unsigned int);

    bitboard knightAttacks(unsigned int);

    bitboard kingAttacks(unsigned int);

    bitboard getFileMaskEx(unsigned int);

    bitboard getFileMask(unsigned int);

    bitboard getRayMask(unsigned int, Direction);

    void update_general_data();

    void update_checks();

    void update_lmg_bitboards(); // legal move generation

    void generate_pawn_pushes(move*);

    void generate_pawn_captures(move*);

    void generate_pawn_promotions(move*);

    void generate_king_captures(move*);

    void generate_bitboards();

    void generate_pawn_moves(move *);

    void generate_king_moves(move *);

    unsigned int generate_moves(move *, bool);

    bitboard pawnAttacks(unsigned int, Color);

    void init();
};

#endif //CHESS_MOVEGEN_H
