#include <cassert>
#include "globals.h"
#include "movegen.h"
#include "utils.h"
#include "board.h"
#include "piece.h"

// lookup tables
bitboard rayMasks[8][64], bitMask[64], fileMask[64], rankMask[64], diagonalMask[64], antiDiagonalMask[64],
        fileMaskEx[64], rankMaskEx[64], diagonalMaskEx[64], antiDiagonalMaskEx[64];
bitboard rankAttackTable[256][8], knightAttackTable[64], kingAttackTable[64], pawnAttackTable[64][2];

unsigned int movecnt=0;

void movegen::init() {
    bitboard north = 0x0101010101010100ULL;
    for (int sq = 0; sq < 64; sq++) {
        bitMask[sq] = 1ULL << sq;
        rayMasks[NORTH][sq] = north;
        north <<= 1;
    }
    bitboard south = 0x80808080808080ULL;
    for (int sq = 63; sq >= 0; sq--) {
        rayMasks[SOUTH][sq] = south;
        south >>= 1;
    }

    bitboard east = 0xfeULL;
    for (int f = 0; f < 8; f++, east = east.step(EAST)) {
        bitboard temp = east;
        for (int r = 0; r < 8; r++, temp = temp.step(NORTH)) {
            rayMasks[EAST][r * 8 + f] = temp;
        }
    }
    bitboard west = 0x7f00000000000000ULL;
    for (int f = 7; f >= 0; f--, west = west.step(WEST)) {
        bitboard temp = west;
        for (int r = 7; r >= 0; r--, temp = temp.step(SOUTH)) {
            rayMasks[WEST][r * 8 + f] = temp;
        }
    }

    bitboard northEast = 0x8040201008040200ULL;
    for (int f = 0; f < 8; f++, northEast = northEast.step(EAST)) {
        bitboard temp = northEast;
        for (int r = 0; r < 8; r++, temp = temp.step(NORTH)) {
            rayMasks[NoEa][r * 8 + f] = temp;
        }
    }
    bitboard northWest = 0x102040810204000ULL;
    for (int f = 7; f >= 0; f--, northWest = northWest.step(WEST)) {
        bitboard temp = northWest;
        for (int r = 0; r < 8; r++, temp = temp.step(NORTH)) {
            rayMasks[NoWe][r * 8 + f] = temp;
        }
    }

    bitboard southEast = 0x2040810204080ULL;
    for (int f = 0; f < 8; f++, southEast = southEast.step(EAST)) {
        bitboard temp = southEast;
        for (int r = 7; r >= 0; r--, temp = temp.step(SOUTH)) {
            rayMasks[SoEa][r * 8 + f] = temp;
        }
    }
    bitboard southWest = 0x40201008040201ULL;
    for (int f = 7; f >= 0; f--, southWest = southWest.step(WEST)) {
        bitboard temp = southWest;
        for (int r = 7; r >= 0; r--, temp = temp.step(SOUTH)) {
            rayMasks[SoWe][r * 8 + f] = temp;
        }
    }

    for (int sq = 0; sq < 64; sq++) {
        fileMaskEx[sq] = rayMasks[NORTH][sq] | rayMasks[SOUTH][sq];
        rankMaskEx[sq] = rayMasks[EAST][sq] | rayMasks[WEST][sq];
        diagonalMaskEx[sq] = rayMasks[NoEa][sq] | rayMasks[SoWe][sq];
        antiDiagonalMaskEx[sq] = rayMasks[NoWe][sq] | rayMasks[SoEa][sq];
        fileMask[sq] = fileMaskEx[sq] | bitMask[sq];
        rankMask[sq] = rankMaskEx[sq] | bitMask[sq];
        diagonalMask[sq] = diagonalMaskEx[sq] | bitMask[sq];
        antiDiagonalMask[sq] = antiDiagonalMask[sq] | bitMask[sq];
    }

    for (unsigned int occ = 0; occ < 256; occ++) {
        for (unsigned int sq = 0; sq < 8; sq++) {
            bitboard attack;
            for (unsigned int j = sq + 1; j < 8; j++) {
                attack.set(j);
                if ((occ >> j) & 1) break;
            }
            for (int j = sq - 1; j >= 0; j--) {
                attack.set(j);
                if ((occ >> j) & 1) break;
            }
            rankAttackTable[occ][sq] = attack;
        }
    }

    for (unsigned int sq = 0; sq < 64; sq++) {
        knightAttackTable[sq] = bitMask[sq].step(NoWe).step(NORTH) | bitMask[sq].step(NoEa).step(NORTH) |
                                bitMask[sq].step(NoWe).step(WEST) | bitMask[sq].step(NoEa).step(EAST) |
                                bitMask[sq].step(SoWe).step(SOUTH) | bitMask[sq].step(SoEa).step(SOUTH) |
                                bitMask[sq].step(SoWe).step(WEST) | bitMask[sq].step(SoEa).step(EAST);
    }

    for (unsigned int sq = 0; sq < 64; sq++) {
        kingAttackTable[sq] =
                bitMask[sq].step(NoWe) | bitMask[sq].step(NORTH) | bitMask[sq].step(NoEa) | bitMask[sq].step(EAST) |
                bitMask[sq].step(SoEa) | bitMask[sq].step(SOUTH) | bitMask[sq].step(SoWe) | bitMask[sq].step(WEST);
    }

    for (unsigned int sq = 0; sq < 64; sq++) {
        pawnAttackTable[sq][0] = bitMask[sq].step(NoWe) | bitMask[sq].step(NoEa);
        pawnAttackTable[sq][1] = bitMask[sq].step(SoWe) | bitMask[sq].step(SoEa);
    }
}

void movegen::update_general_data() {
    b.occupied = b.allPieceBB[0] | b.allPieceBB[1];
    b.empty = ~b.occupied;
    b.kingDanger = 0;
    b.pinned = 0;
    b.kingPos = (b.stm == WHITE ? b.wking : b.bking);
}

void movegen::update_checks() {
    Color op = (b.stm == WHITE ? BLACK : WHITE);
    b.checkers = (pawnAttacks(b.kingPos, b.stm) & (b.allPieceBB[op] & b.pieceBB[PAWN])) |
                 (rookAttacks(b.kingPos) & (b.allPieceBB[op] & b.pieceBB[ROOK])) |
                 (bishopAttacks(b.kingPos) & (b.allPieceBB[op] & b.pieceBB[BISHOP])) |
                 (queenAttacks(b.kingPos) & (b.allPieceBB[op] & b.pieceBB[QUEEN])) |
                 (knightAttacks(b.kingPos) & (b.allPieceBB[op] & b.pieceBB[KNIGHT]));

    b.checks = b.checkers.pop_cnt();
}

void movegen::update_lmg_bitboards() {
    Color op = (b.stm == WHITE ? BLACK : WHITE);

    bitboard enemyPieces = b.allPieceBB[op];
    b.occupied.clear(b.kingPos);
    while (enemyPieces) {
        unsigned int from = enemyPieces.pop_lsb();
        bitboard attacks;
        switch (b.pieces[from]) {
            case ROOK:
                attacks |= rookAttacks(from);
                break;
            case KNIGHT:
                attacks |= knightAttacks(from);
                break;
            case BISHOP:
                attacks |= bishopAttacks(from);
                break;
            case QUEEN:
                attacks |= queenAttacks(from);
                break;
            case KING:
                attacks |= kingAttacks(from);
                break;
            case PAWN:
                attacks |= pawnAttacks(from, op);
                break;
        }
        b.kingDanger |= attacks;
        if (b.pieces[from] == ROOK || b.pieces[from] == BISHOP || b.pieces[from] == QUEEN) {
            bitboard attackedPieces = attacks & b.allPieceBB[b.stm];
            if (attackedPieces.get(b.kingPos)) continue;
            b.occupied ^= attackedPieces;
            bitboard xrayedSquares;
            if (b.pieces[from] == ROOK || b.pieces[from] == QUEEN)
                xrayedSquares |= rookAttacks(from);
            if (b.pieces[from] == BISHOP || b.pieces[from] == QUEEN)
                xrayedSquares |= bishopAttacks(from);
            if (xrayedSquares.get(b.kingPos)) {
                bitboard ray = getRayIntoSq(b.kingPos, from);
                unsigned int pinnedSq = (ray & attackedPieces).lsb();
                b.pinMasks[pinnedSq] = ray;
                b.pinned.set(pinnedSq);
            }
            b.occupied ^= attackedPieces;
        }
    }
    b.occupied.set(b.kingPos);

    // valid move masks
    b.pushMask = 0xffffffffffffffffULL;
    b.captureMask = 0xffffffffffffffffULL;
    if (b.checks == 1) {
        unsigned int pos = b.checkers.lsb();
        if (b.pieces[pos] == ROOK || b.pieces[pos] == BISHOP || b.pieces[pos] == QUEEN) {
            b.pushMask = getRayIntoSq(pos,
                                      b.kingPos); // we know that the ray is the element of queenAttack, so we can use it to handle collisions
            b.pushMask.clear(b.kingPos);
        } else {
            b.pushMask = 0;
        }
        b.captureMask = b.checkers;

        if (b.ep.top() != -1 && b.pushMask.get(b.ep.top())) {
            b.captureMask.set(b.ep.top()); // only pawns will be able to capture that square
        }
        if (b.ep.top() != -1 && b.captureMask.get(b.ep.top() + (b.stm == WHITE ? -8 : 8))) {
            b.captureMask.set(b.ep.top()); // only pawns will be able to capture that square
        }
    } else if (b.checks >
               1) { // I understand that triple check is not possible, but just to be sure custom pos doesn't break this
        b.captureMask = 0;
        b.pushMask = 0;
    }
}

void movegen::generate_bitboards() {
    assert(b.pieces[b.wking] == KING);
    assert(b.pieces[b.bking] == KING);
    update_general_data();
    update_checks();
    update_lmg_bitboards();
}

void movegen::generate_pawn_pushes(move *moves) {
    if (b.stm == WHITE) {
        bitboard wpawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];
        bitboard singlePawnPush = (wpawns & notRank7).step(NORTH) & b.empty;
        bitboard doublePawnPush = (singlePawnPush & rank3).step(NORTH) & b.empty & b.pushMask;
        singlePawnPush &= b.pushMask;

        while (singlePawnPush) {
            unsigned int to = singlePawnPush.pop_lsb();
            if (b.pinned.get(to - 8) && !b.pinMasks[to - 8].get(to)) continue;
            moves[movecnt++] = move(to - 8, to, 0);
        }

        while (doublePawnPush) {
            unsigned int to = doublePawnPush.pop_lsb();
            if (b.pinned.get(to - 16) && !b.pinMasks[to - 16].get(to)) continue;
            moves[movecnt++] = move(to - 16, to, SPECIAL2_FLAG);
        }
    } else {
        bitboard bpawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];
        bitboard singlePawnPush = (bpawns & notRank2).step(SOUTH) & b.empty;
        bitboard doublePawnPush = (singlePawnPush & rank6).step(SOUTH) & b.empty & b.pushMask;
        singlePawnPush &= b.pushMask;

        while (singlePawnPush) {
            unsigned int to = singlePawnPush.pop_lsb();
            if (b.pinned.get(to + 8) && !b.pinMasks[to + 8].get(to)) continue;
            moves[movecnt++] = move(to + 8, to, 0);
        }

        while (doublePawnPush) {
            unsigned int to = doublePawnPush.pop_lsb();
            if (b.pinned.get(to + 16) && !b.pinMasks[to + 16].get(to)) continue;
            moves[movecnt++] = move(to + 16, to, SPECIAL2_FLAG);
        }
    }
}

void movegen::generate_pawn_captures(move *moves) {
    if (b.stm == WHITE) {
        if (b.ep.top() != -1) {
            b.allPieceBB[BLACK].set(b.ep.top());
        }

        bitboard wpawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];
        bitboard captureLeft = (wpawns & notRank7).step(NoWe) & b.allPieceBB[BLACK] & b.captureMask;
        bitboard captureRight = (wpawns & notRank7).step(NoEa) & b.allPieceBB[BLACK] & b.captureMask;

        while (captureLeft) {
            unsigned int to = captureLeft.pop_lsb();
            if (b.pinned.get(to - 7) && !b.pinMasks[to - 7].get(to)) continue;
            if (to == b.ep.top()) {
                b.occupied ^= bitMask[to - 8] | bitMask[to - 7]; // special case see 8/8/8/K2pP2r/8/8/5k2/8 w - d6 0 1
                bitboard rankAttack = rankAttacks(to - 8);
                bitboard rankCheck = rankAttack & b.allPieceBB[BLACK] & (b.pieceBB[ROOK] | b.pieceBB[QUEEN]);
                b.occupied ^= bitMask[to - 8] | bitMask[to - 7];
                if (rankCheck && rankAttack.get(b.wking)) continue;
                moves[movecnt++] = move(to - 7, to, CAPTURE_FLAG | SPECIAL2_FLAG, encodePiece(BLACK, PAWN));
            } else {
                moves[movecnt++] = move(to - 7, to, CAPTURE_FLAG, encodePiece(BLACK, b.pieces[to]));
            }
        }

        while (captureRight) {
            unsigned int to = captureRight.pop_lsb();
            if (b.pinned.get(to - 9) && !b.pinMasks[to - 9].get(to)) continue;
            if (to == b.ep.top()) {
                b.occupied ^= bitMask[to - 8] | bitMask[to - 9]; // special case see 8/8/8/K2pP2r/8/8/5k2/8 w - d6 0 1
                bitboard rankAttack = rankAttacks(to - 8);
                bitboard rankCheck = rankAttack & b.allPieceBB[BLACK] & (b.pieceBB[ROOK] | b.pieceBB[QUEEN]);
                b.occupied ^= bitMask[to - 8] | bitMask[to - 9];
                if (rankCheck && rankAttack.get(b.wking)) continue;
                moves[movecnt++] = move(to - 9, to, CAPTURE_FLAG | SPECIAL2_FLAG, encodePiece(BLACK, PAWN));
            } else {
                moves[movecnt++] = move(to - 9, to, CAPTURE_FLAG, encodePiece(BLACK, b.pieces[to]));
            }
        }

        if (b.ep.top() != -1) {
            b.allPieceBB[BLACK].clear(b.ep.top());
        }
    }
    else {
        if (b.ep.top() != -1) {
            b.allPieceBB[WHITE].set(b.ep.top());
        }

        bitboard bpawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];
        bitboard captureLeft = (bpawns & notRank2).step(SoWe) & b.allPieceBB[WHITE] & b.captureMask;
        bitboard captureRight = (bpawns & notRank2).step(SoEa) & b.allPieceBB[WHITE] & b.captureMask;

        while (captureLeft) {
            unsigned int to = captureLeft.pop_lsb();
            if (b.pinned.get(to + 9) && !b.pinMasks[to + 9].get(to)) continue;
            if (to == b.ep.top()) {
                b.occupied ^= bitMask[to + 8] | bitMask[to + 9]; // special case see 8/8/8/K2pP2r/8/8/5k2/8 w - d6 0 1
                bitboard rankAttack = rankAttacks(to + 8);
                bitboard rankCheck = rankAttack & b.allPieceBB[WHITE] & (b.pieceBB[ROOK] | b.pieceBB[QUEEN]);
                b.occupied ^= bitMask[to + 8] | bitMask[to + 9];
                if (rankCheck && rankAttack.get(b.bking)) continue;
                moves[movecnt++] = move(to + 9, to, CAPTURE_FLAG | SPECIAL2_FLAG, encodePiece(WHITE, PAWN));
            } else {
                moves[movecnt++] = move(to + 9, to, CAPTURE_FLAG, encodePiece(WHITE, b.pieces[to]));
            }
        }

        while (captureRight) {
            unsigned int to = captureRight.pop_lsb();
            if (b.pinned.get(to + 7) && !b.pinMasks[to + 7].get(to)) continue;
            if (to == b.ep.top()) {
                b.occupied ^= bitMask[to + 8] | bitMask[to + 7]; // special case see 8/8/8/K2pP2r/8/8/5k2/8 w - d6 0 1
                bitboard rankAttack = rankAttacks(to + 8);
                bitboard rankCheck = rankAttack & b.allPieceBB[WHITE] & (b.pieceBB[ROOK] | b.pieceBB[QUEEN]);
                b.occupied ^= bitMask[to + 8] | bitMask[to + 7];
                if (rankCheck && rankAttack.get(b.bking)) continue;
                moves[movecnt++] = move(to + 7, to, CAPTURE_FLAG | SPECIAL2_FLAG, encodePiece(WHITE, b.pieces[to]));
            } else {
                moves[movecnt++] = move(to + 7, to, CAPTURE_FLAG, encodePiece(WHITE, b.pieces[to]));
            }
        }

        if (b.ep.top() != -1) {
            b.allPieceBB[WHITE].clear(b.ep.top());
        }
    }
}

void movegen::generate_pawn_promotions(move *moves) {
    if (b.stm == WHITE) {
        bitboard wpawns = b.allPieceBB[WHITE] & b.pieceBB[PAWN];

        if (wpawns & rank7) {
            bitboard promoLeft = (wpawns & rank7 & ~b.pinned).step(NoWe) & b.allPieceBB[BLACK] & b.captureMask;
            bitboard promoRight = (wpawns & rank7 & ~b.pinned).step(NoEa) & b.allPieceBB[BLACK] & b.captureMask;
            bitboard promoUp = (wpawns & rank7 & ~b.pinned).step(NORTH) & b.empty & b.pushMask;

            while (promoLeft) {
                unsigned int to = promoLeft.pop_lsb();
                moves[movecnt++] = move(to - 7, to, CAPTURE_FLAG | PROMO_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] =
                        move(to - 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] =
                        move(to - 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL2_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] = move(to - 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG,
                                        encodePiece(BLACK, b.pieces[to]));
            }

            while (promoRight) {
                unsigned int to = promoRight.pop_lsb();
                moves[movecnt++] = move(to - 9, to, CAPTURE_FLAG | PROMO_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] =
                        move(to - 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] =
                        move(to - 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL2_FLAG, encodePiece(BLACK, b.pieces[to]));
                moves[movecnt++] = move(to - 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG,
                                        encodePiece(BLACK, b.pieces[to]));
            }

            while (promoUp) {
                unsigned int to = promoUp.pop_lsb();
                moves[movecnt++] = move(to - 8, to, PROMO_FLAG);
                moves[movecnt++] = move(to - 8, to, PROMO_FLAG | SPECIAL1_FLAG);
                moves[movecnt++] = move(to - 8, to, PROMO_FLAG | SPECIAL2_FLAG);
                moves[movecnt++] = move(to - 8, to, PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG);
            }
        }
    } else {
        bitboard bpawns = b.allPieceBB[BLACK] & b.pieceBB[PAWN];

        if (bpawns & rank2) {
            bitboard promoLeft = (bpawns & rank2 & ~b.pinned).step(SoWe) & b.allPieceBB[WHITE] & b.captureMask;
            bitboard promoRight = (bpawns & rank2 & ~b.pinned).step(SoEa) & b.allPieceBB[WHITE] & b.captureMask;
            bitboard promoDown = (bpawns & rank2 & ~b.pinned).step(SOUTH) & b.empty & b.pushMask;
            while (promoLeft) {
                unsigned int to = promoLeft.pop_lsb();
                moves[movecnt++] = move(to + 9, to, CAPTURE_FLAG | PROMO_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] =
                        move(to + 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] =
                        move(to + 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL2_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] = move(to + 9, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG,
                                        encodePiece(WHITE, b.pieces[to]));
            }

            while (promoRight) {
                unsigned int to = promoRight.pop_lsb();
                moves[movecnt++] = move(to + 7, to, CAPTURE_FLAG | PROMO_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] =
                        move(to + 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] =
                        move(to + 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL2_FLAG, encodePiece(WHITE, b.pieces[to]));
                moves[movecnt++] = move(to + 7, to, CAPTURE_FLAG | PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG,
                                        encodePiece(WHITE, b.pieces[to]));
            }

            while (promoDown) {
                unsigned int to = promoDown.pop_lsb();
                moves[movecnt++] = move(to + 8, to, PROMO_FLAG);
                moves[movecnt++] = move(to + 8, to, PROMO_FLAG | SPECIAL1_FLAG);
                moves[movecnt++] = move(to + 8, to, PROMO_FLAG | SPECIAL2_FLAG);
                moves[movecnt++] = move(to + 8, to, PROMO_FLAG | SPECIAL1_FLAG | SPECIAL2_FLAG);
            }
        }
    }
}

void movegen::generate_pawn_moves(move *moves) {
    generate_pawn_pushes(moves);
    generate_pawn_captures(moves);
    generate_pawn_promotions(moves);
}

void movegen::generate_king_captures(move *moves) {
    bitboard captures = kingAttacks(b.kingPos) & ~b.allPieceBB[b.stm] & b.occupied;
    while (captures) {
        unsigned int to = captures.pop_lsb();
        moves[movecnt++] = move(b.kingPos, to, CAPTURE_FLAG, encodePiece(b.colors[to], b.pieces[to]));
    }
}

void movegen::generate_king_moves(move *moves) {
    bitboard attacks = kingAttacks(b.kingPos) & ~b.allPieceBB[b.stm];
    bitboard captures = attacks & b.occupied;
    attacks &= ~captures;
    while (attacks) {
        unsigned int to = attacks.pop_lsb();
        moves[movecnt++] = move(b.kingPos, to, 0);
    }
    while (captures) {
        unsigned int to = captures.pop_lsb();
        moves[movecnt++] = move(b.kingPos, to, CAPTURE_FLAG, encodePiece(b.colors[to], b.pieces[to]));
    }
}

bitboard movegen::pawnAttacks(unsigned int sq, Color color)  {
    return pawnAttackTable[sq][color];
}

bitboard movegen::fileAttacks(unsigned int sq) {
    // The math behind these this functions was taken from https://www.chessprogramming.org/Hyperbola_Quintessence
    bitboard attacks(b.occupied & fileMaskEx[sq]);
    bitboard reversedAttacks = attacks.reverse();
    attacks = attacks - bitMask[sq];
    reversedAttacks = reversedAttacks - bitMask[sq].reverse();
    attacks = attacks ^ reversedAttacks.reverse();
    return attacks & fileMaskEx[sq];
}

bitboard movegen::rankAttacks(unsigned int sq) {
    bitboard key = (b.occupied & rankMask[sq]) >> indexToRank(sq) * 8;
    bitboard attacks = rankAttackTable[(unsigned int) key][indexToFile(sq)] << indexToRank(sq) * 8;
    return attacks;
}

bitboard movegen::diagonalAttacks(unsigned int sq)  {
    // The math behind these this functions was taken from https://www.chessprogramming.org/Hyperbola_Quintessence
    bitboard attacks(b.occupied & diagonalMaskEx[sq]);
    bitboard reversedAttacks = attacks.reverse();
    attacks = attacks - bitMask[sq];
    reversedAttacks = reversedAttacks - bitMask[sq].reverse();
    attacks = attacks ^ reversedAttacks.reverse();
    return attacks & diagonalMaskEx[sq];
}

bitboard movegen::antiDiagonalAttacks(unsigned int sq)  {
    // The math behind these this functions was taken from https://www.chessprogramming.org/Hyperbola_Quintessence
    bitboard attacks(b.occupied & antiDiagonalMaskEx[sq]);
    bitboard reversedAttacks = attacks.reverse();
    attacks = attacks - bitMask[sq];
    reversedAttacks = reversedAttacks - bitMask[sq].reverse();
    attacks = attacks ^ reversedAttacks.reverse();
    return attacks & antiDiagonalMaskEx[sq];
}

bitboard movegen::rookAttacks(unsigned int sq)  {
    return fileAttacks(sq) | rankAttacks(sq);
}

bitboard movegen::bishopAttacks(unsigned int sq)  {
    return diagonalAttacks(sq) | antiDiagonalAttacks(sq);
}

bitboard movegen::queenAttacks(unsigned int sq)  {
    return rookAttacks(sq) | bishopAttacks(sq);
}

bitboard movegen::knightAttacks(unsigned int sq)  {
    return knightAttackTable[sq];
}

bitboard movegen::kingAttacks(unsigned int sq)  {
    return kingAttackTable[sq] & ~b.kingDanger;
}

bitboard movegen::getRayIntoSq(unsigned int from, unsigned int to)  { // TODO make this faster
    for (auto &rayMask: rayMasks) {
        if (rayMask[from].get(to)) return rayMask[from] & queenAttacks(from);
    }
}

unsigned int movegen::generate_moves(move *moves, bool capturesOnly) {
    movecnt = 0;
    generate_bitboards();
    if (capturesOnly) generate_king_captures(moves);
    else generate_king_moves(moves);

    if (b.checks > 1) {
        return movecnt;
    }

    if (capturesOnly)
        generate_pawn_captures(moves);
    else
        generate_pawn_moves(moves);

    Color op = (b.stm == WHITE ? BLACK : WHITE);
    bitboard pieces = b.allPieceBB[b.stm] & ~b.pieceBB[PAWN];
    while (pieces) {
        unsigned int from = pieces.pop_lsb();
        bitboard attacks;
        switch (b.pieces[from]) {
            case ROOK:
                attacks = rookAttacks(from);
                break;
            case KNIGHT:
                attacks = knightAttacks(from);
                break;
            case BISHOP:
                attacks = bishopAttacks(from);
                break;
            case QUEEN:
                attacks = queenAttacks(from);
                break;
        }
        attacks &= ~b.allPieceBB[b.stm];
        if (b.pinned.get(from)) attacks &= b.pinMasks[from];
        bitboard captures = attacks & b.allPieceBB[op] & b.captureMask;
        attacks &= ~captures & b.pushMask;
        if (!capturesOnly) {
            while (attacks) {
                unsigned int to = attacks.pop_lsb();
                moves[movecnt++] = move(from, to, 0);
            }
        }
        while (captures) {
            unsigned int to = captures.pop_lsb();
            moves[movecnt++] = move(from, to, CAPTURE_FLAG, encodePiece(op, b.pieces[to]));
        }
    }
    if (!capturesOnly) {
        // castles
        if (b.stm == WHITE) {
            if (b.canCastle(WK_MASK) && !(b.kingDanger & 112ULL) && !(b.occupied & 96ULL)) {
                moves[movecnt++] = move(4, 6, SPECIAL1_FLAG);
            }
            if (b.canCastle(WQ_MASK) && !(b.kingDanger & 28ULL) && !(b.occupied & 14ULL)) {
                moves[movecnt++] = move(4, 2, SPECIAL1_FLAG | SPECIAL2_FLAG);
            }
        } else {
            if (b.canCastle(BK_MASK) && !(b.kingDanger & 0x7000000000000000ULL) && !(b.occupied & 0x6000000000000000ULL)) {
                moves[movecnt++] = move(60, 62, SPECIAL1_FLAG);
            }
            if (b.canCastle(BQ_MASK) && !(b.kingDanger & 0x1c00000000000000ULL) && !(b.occupied & 0xe00000000000000ULL)) {
                moves[movecnt++] = move(60, 58, SPECIAL1_FLAG | SPECIAL2_FLAG);
            }
        }
    }

    if (movecnt == 0) {
        if (b.checks >= 1) {
            b.status = CHECKMATE;
        } else {
            b.status = DRAW;
        }
    }

    return movecnt;
}

