#include <iostream>
#include <string>
#include "board.h"
#include "uci.h"
#include "bench.h"

board b;
bool stopSearch = false;
int nodeCount = 0;

const unsigned int seed = 3113159740;

int main(int argc, char** argv) {
    srand(seed);
    std::string mode;
    if (argc >= 2) {
        mode = std::string(argv[1]);
    }

    if (mode.empty()) {
        std::cin >> mode;
    }

    if (mode == "uci") {
        uci::loop();
    } else if (mode == "bench") {
        bench::init();
        bench::searchTest(true);
    } else if (mode == "perft") {
        bench::init();
        bench::perftTest();
    } else {
        std::cout << "Invalid mode!" << std::endl;
    }
    return 0;
}
