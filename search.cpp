#include <chrono>
#include <cassert>
#include "search.h"
#include "globals.h"
#include "eval.h"
#include "movegen.h"
#include "move_ordering.h"
#include "transposition_table.h"
#include "uci.h"

int searchCaptures(int alpha, int beta, int ply) { // TODO maybe record somehow these values too?
    if (stopSearch) return UNKNOWN_EVAL;
    nodeCount++;

    int curr_eval = eval();
    if (curr_eval >= beta) return beta;
    if (alpha < curr_eval) alpha = curr_eval;

    move moves[200];
    unsigned int moveCount = movegen::generate_moves(moves, true);

    // move ordering
    orderMoves(moves, moveCount);

    for (int i = 0; i < moveCount; i++) {

        b.makeMove(moves[i]);
        int score = -searchCaptures(-beta, -alpha, ply + 1);
        b.undoMove();

        if (stopSearch) return UNKNOWN_EVAL;
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;

    }

    return alpha;
}

int search(unsigned int depth, int alpha, int beta, int ply) {
    if (stopSearch) return UNKNOWN_EVAL;
    nodeCount++;

    // check if we have an entry of this position in tt
    int value = probeHash(depth, alpha, beta);
    if (value != UNKNOWN_EVAL) {
        return value;
    }

    // Quiescence Search
    if (depth == 0) {
        return searchCaptures(alpha, beta, ply);
    }

    move moves[200];
    unsigned int moveCount = movegen::generate_moves(moves, false);

    // checking if the game has ended
    if (b.status != PLAYING) {
        if (b.status == CHECKMATE) {
            return -MATE_SCORE + ply; // we add distance from the root node so the engine prefers shorter mates
        }
        if (b.status == DRAW) {
            return 0; // TODO
        }
    }

    EntryFlag flag = ALPHA;

    // move ordering
    orderMoves(moves, moveCount);

    // mate distance pruning
    // this is useful because we disable tt with mate scores so this gets the speed back
    int lowerBound = -MATE_SCORE + ply, upperBound = MATE_SCORE - ply;
    if (alpha < lowerBound) {
        alpha = lowerBound;
        if (beta <= lowerBound) return lowerBound;
    }
    if (beta > upperBound) {
        beta = upperBound;
        if (alpha >= upperBound) return upperBound;
    }


    // iterating through moves
    move bestMove;
    for (int i = 0; i < moveCount; i++) {
        b.makeMove(moves[i]);
        int score = -search(depth - 1, -beta, -alpha, ply + 1);
        b.undoMove();

        if (stopSearch) return UNKNOWN_EVAL;

        if (score >= beta) {
            saveHash(depth, moves[i], beta, BETA);
            return beta;
        }

        if (score > alpha) {
            alpha = score;
            bestMove = moves[i];
            flag = EXACT;
        }
    }

    saveHash(depth, bestMove, alpha, flag);
    return alpha;
}

move searchRoot(unsigned int depth) {
    nodeCount = 1;
    move bestMove;
    int bestEval = INT32_MIN;

    move moves[200];
    unsigned int moveCount = movegen::generate_moves(moves, false);

    // move ordering
    orderMoves(moves, moveCount);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (int i = 0; i < moveCount; i++) {

        b.makeMove(moves[i]);
        int e = -search(depth, -INF, INF, 1);
        b.undoMove();

        if (stopSearch) return {};

        if (e > bestEval) {
            bestEval = e;
            bestMove = moves[i];
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // currently only supports one move
    std::string pvLine = bestMove.str();

    // providing statistics to the GUI
    long long milli = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    long long nps = milli == 0 ? 0 : nodeCount / milli * 1000;

    std::string scoreStr = "score ";

    int mateDistance = std::abs(std::abs(bestEval) - MATE_SCORE) / 2 + 1; // TODO fix this

    if (mateDistance < 100) {
        scoreStr += "mate " + std::to_string(mateDistance);
    } else {
        scoreStr += "cp " + std::to_string(bestEval);
    }

    uci::out("info",
             {"depth", std::to_string(depth), "nodes", std::to_string(nodeCount), scoreStr,
              "time",
              std::to_string(milli), "nps", std::to_string(nps), "pv", pvLine});

    return bestMove;
}

void iterativeDeepening(unsigned int maxDepth) {
    stopSearch = false;
    move bestMove;

    for (unsigned int depth = 1; depth <= maxDepth; depth++) {
        move pvMove = searchRoot(depth);

        if (stopSearch) {
            break;
        }

        if (!pvMove.isNULL()) {
            bestMove = pvMove;
        }
    }

    stopSearch = true;

    uci::out("bestmove", {bestMove.str()});
}