//
// Created by szil on 2022.07.09..
//

#include <utility>
#include <algorithm>
#include <cassert>
#include "constants.h"
#include "globals.h"
#include "move_ordering.h"
#include "transposition_table.h"

int basePieceValues[6] = {100, 600, 320, 380, 1050};

void orderMoves(move *moves, unsigned int moveCount) {
    std::vector<std::pair<int, move>> move_list(moveCount);
    move recordedBestMove = getBestMove();
    for (int i = 0; i < moveCount; i++) {
        int score = 0;
        if (moves[i] == recordedBestMove) score += 10000;
        if (moves[i].isPromo()) {
            if (moves[i].isSpecial1() && moves[i].isSpecial2()) score += basePieceValues[QUEEN];
            else if (moves[i].isSpecial1() && !moves[i].isSpecial2()) score += basePieceValues[ROOK];
            else if (!moves[i].isSpecial1() && moves[i].isSpecial2()) score += basePieceValues[BISHOP];
            else score += basePieceValues[KNIGHT];

        }
        if (moves[i].isCapture()) {
            score += basePieceValues[decodePiece(moves[i].getCapturedPiece()).second];
        }
        move_list[i] = {score, moves[i]};
    }

    std::sort(move_list.begin(), move_list.end());
    std::reverse(move_list.begin(), move_list.end());
    for (int i = 0; i < moveCount; i++) {
        moves[i] = move_list[i].second;
    }
}