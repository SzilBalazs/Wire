#include <utility>
#include <algorithm>
#include <cstring>
#include "globals.h"
#include "move_ordering.h"
#include "transposition_table.h"

int basePieceValues[6] = {100, 600, 320, 380, 1050};

move killerMoves[101][2];

void clearKillerMoves() {
    std::memset(killerMoves, 0, sizeof(killerMoves));
}

void recordKillerMove(move m, int ply) {
    killerMoves[ply][1] = killerMoves[ply][0];
    killerMoves[ply][0] = m;
}

void orderMoves(move *moves, unsigned int moveCount, int ply) {
    std::vector<std::pair<int, move>> move_list(moveCount);
    move hashMove = getBestMove();
    for (int i = 0; i < moveCount; i++) {
        int score = 0;

        // Hash move
        if (moves[i] == hashMove) score += 10000;

        // Promotion
        else if (moves[i].isPromo()) {
            if (moves[i].isSpecial1() && moves[i].isSpecial2()) score += 9400; // QUEEN
            else if (moves[i].isSpecial1() && !moves[i].isSpecial2()) score += 9200; // ROOK
            else if (!moves[i].isSpecial1() && moves[i].isSpecial2()) score += 9100; // BISHOP
            else score += 9300; // KNIGHT
        }

        // Killer moves
        else if (moves[i] == killerMoves[ply][0]) score += 6000;
        else if (moves[i] == killerMoves[ply][1]) score += 5000;

        // Captures
        else if (moves[i].isCapture()) {
            int attacker = basePieceValues[b.pieces[moves[i].getFrom()]];
            int victim = basePieceValues[decodePiece(moves[i].getCapturedPiece()).second];
            if (attacker < victim) score += 8000;
            else if (attacker == victim) score += 7000;
            else score += 4000;
        }


        move_list[i] = {score, moves[i]};
    }

    std::sort(move_list.begin(), move_list.end());
    std::reverse(move_list.begin(), move_list.end());
    for (int i = 0; i < moveCount; i++) {
        moves[i] = move_list[i].second;
    }
}