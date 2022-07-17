#include <iostream>
#include <string>
#include <chrono>
#include <cassert>
#include "globals.h"
#include "board.h"
#include "search.h"
#include "move.h"
#include "movegen.h"
#include "uci.h"
#include "eval.h"

board b;
bool stopSearch = false;
int nodeCount=0;


int perft(int depth) {
    move moves[150];
    unsigned int movecnt = movegen::generate_moves(moves, false);
    if (depth == 1) return movecnt;
    int s = 0;
    for (unsigned int i = 0; i < movecnt; i++) {
        U64 a = b.getHash();
        b.makeMove(moves[i]);
        s += perft(depth - 1);
        b.undoMove();
        assert(a == b.getHash());
    }
    return s;
}

int main() {
    srand(time(nullptr));

    std::string mode;
    std::cin >> mode;
    if (mode == "uci") {
        uci::loop();
    }

    return 0;
    std::string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    b.loadPositionFromFEN(start_fen);
    hash_init();
    movegen::init();
    for (int i=0;i<500;i++) {
        system("cls");
        b.displayBoard();
        std::cout << "\n\nPositions evaluated = " << nodeCount << std::endl << "Current eval = " << eval() << std::endl;
        bool valid=false;
        move playerMove;
        while (!valid) {
            std::cout << "\nMake your move: ";
            std::string s;
            std::cin >> s;
            unsigned int start = (s[0] - 'a') + (s[1] - '1') * 8;
            unsigned int end = (s[2] - 'a') + (s[3] - '1') * 8;
            move moves[200];
            unsigned int movecnt = movegen::generate_moves(moves, 0);
            for (int i=0;i<movecnt;i++) {
                if (moves[i].getFrom() == start && moves[i].getTo() == end) {
                    playerMove = moves[i];
                    valid = true;
                }
            }
            if (!valid) std::cout << "Invalid!\n";
        }
        b.makeMove(playerMove);
        system("cls");
        b.displayBoard();
        std::cout << "Thinking...\n";
        for (int i=1;i<6;i++) {
            searchRoot(i);
        }
        move m = searchRoot(6);
        b.makeMove(m);
    }
    return 0;
}
