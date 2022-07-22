#ifndef CHESS_UCI_H
#define CHESS_UCI_H

#include <iostream>
#include <vector>
#include "constants.h"

namespace uci {
    void out(std::string cmd);

    void out(std::string cmd, std::vector<std::string> params);

    void logMessage(std::string message);

    void go();

    void init();

    void loop();
}


#endif //CHESS_UCI_H
