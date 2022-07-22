#include <iostream>
#include <string>
#include "board.h"
#include "uci.h"
#include "bench.h"

board b;
bool stopSearch = false;
int nodeCount = 0;


int main() {
    std::string mode;
    std::cin >> mode;

    if (mode == "uci") {
        uci::loop();
    } else if (mode == "bench") {
        bench::init();
        bench::searchTest();
    } else if (mode == "perft") {
        bench::init();
        bench::perftTest();
    } else {
        std::cout << "Invalid mode!" << std::endl;
    }
    return 0;
}
