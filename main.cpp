#include <iostream>
#include <string>
#include "board.h"
#include "uci.h"

board b;
bool stopSearch = false;
int nodeCount=0;


int main() {
    std::string mode;
    std::cin >> mode;

    if (mode == "uci") {
        uci::loop();
    }

    std::cout << "Invalid protocol!" << std::endl;
    return 0;
}
