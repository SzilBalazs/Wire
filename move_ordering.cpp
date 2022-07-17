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

int basePieceValues[6] = {90, 500, 260, 300, 900};

void orderMoves(move *moves, unsigned int movecnt) {
    std::vector<std::pair<int, move>> move_list(movecnt);
    move recordedBestMove = getBestMove();
    for (int i=0;i<movecnt;i++) {
        int score = 0;
        if (moves[i] == recordedBestMove) score+=10000;
        if (moves[i].isPromo()) score+=900;
        if (moves[i].isCapture()) {
            score += basePieceValues[decodePiece(moves[i].getCapturedPiece()).second];
        }
        move_list[i] = {score, moves[i]};
    }

    std::sort(move_list.begin(), move_list.end());
    std::reverse(move_list.begin(), move_list.end());
    for (int i=0;i<movecnt;i++) {
        moves[i] = move_list[i].second;
    }
}