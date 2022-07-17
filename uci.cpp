#include <sstream>
#include <thread>
#include <fstream>
#include "uci.h"
#include "board.h"
#include "movegen.h"
#include "globals.h"
#include "search.h"

namespace uci {

    bool debugMode;
    unsigned int maxDepth, wTime, bTime, nextTimeControl;

    void out(std::string cmd) {
        std::cout << cmd << "\n";
    }

    void out(std::string cmd, std::vector<std::string> params) {
        std::cout << cmd;
        for (std::string str: params) {
            std::cout << " " << str;
        }
        std::cout << std::endl;
    }

    void init() {
        hashInit();
        movegen::init();
    }

    void logMessage(std::string message) {
        if (debugMode) {
            out("info", {"string", message});
        }
    }

    void waitAndStopSearch(unsigned int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        stopSearch = true;
    }

    void go() {
        unsigned int remainingTime = (b.stm == WHITE ? wTime : bTime);
        std::thread searchStopThread;
        if (remainingTime != 0) {
            unsigned int searchLength = remainingTime/(nextTimeControl+2);
            searchStopThread = std::thread(waitAndStopSearch, searchLength);
        }

        iterativeDeepening(maxDepth);

        if (searchStopThread.joinable())
            searchStopThread.join();
    }

    void loop() {
        debugMode = false;
        stopSearch = true;

        out("id", {"name", ENGINE_NAME});
        out("id", {"author", AUTHOR});
        out("uciok");
        init();
        std::thread searchThread;
        while (true) {
            std::string cmd;
            std::cin >> cmd;
            if (cmd == "debug") {
                std::string type;
                std::cin >> type;
                if (type == "on") {
                    debugMode = true;
                    logMessage("Debug mode turned on");
                } else if (type == "off") {
                    logMessage("Debug mode turned off");
                    debugMode = false;
                }
            }
            else if (cmd == "isready") {
                out("readyok");
            } else if (cmd == "setoption") {
                // TODO
            } else if (cmd == "ucinewgame") {

            } else if (cmd == "position" || cmd == "pos") {
                std::string type;
                std::cin >> type;
                if (type == "startpos") {
                    std::string line, token;
                    std::getline(std::cin, line);
                    b.loadPositionFromFEN(STARTING_POSITION);
                    unsigned int movesStart = 0;
                    while (movesStart < line.size() && line[movesStart] != 'm') movesStart++;
                    std::stringstream ss;
                    ss << line;
                    ss >> token;
                    if (token == "moves") {
                        while (ss) {
                            std::string str_move;
                            ss >> str_move;
                            if (str_move.empty()) break;
                            move m = stringToMove(str_move);
                            b.makeMove(m);
                        }
                    }
                } else if (type == "fen") {
                    std::string line, token;
                    std::getline(std::cin, line);
                    unsigned int movesStart = 0;
                    while (movesStart < line.size() && line[movesStart] != 'm') movesStart++;
                    b.loadPositionFromFEN(line.substr(1, movesStart)); // 1 because there is a space at the start
                    std::stringstream ss;
                    ss << line.substr(movesStart, line.size() - movesStart);
                    ss >> token;
                    if (token == "moves") {
                        while (ss) {
                            std::string str_move;
                            ss >> str_move;
                            if (str_move.empty()) break;
                            move m = stringToMove(str_move);
                            b.makeMove(m);
                        }
                    }
                } else {
                    logMessage("Invalid position flag!");
                }
            } else if (cmd == "go") {
                if (!stopSearch) {
                    logMessage("Search already running, use stop to start a new one!");
                    continue;
                }
                else if (searchThread.joinable()) {
                    searchThread.join();
                }
                std::stringstream ss;
                std::string line;
                std::getline(std::cin, line);
                ss << line;
                // if next time control is not defined (sudden death) we calculate with 60 more moves
                // if wTime or bTime is set to 0 we start an infinite search
                maxDepth=1000, wTime=0, bTime=0, nextTimeControl=60;
                while (ss) {
                    std::string token;
                    ss >> token;
                    logMessage(token);
                    if (token.empty()) break;
                    else if (token == "depth") {
                        ss >> maxDepth;
                    } else if (token == "infinite") {
                        maxDepth = 1000;
                    } else if (token == "wtime") {
                        ss >> wTime;
                    } else if (token == "btime") {
                        ss >> bTime;
                    } else if (token == "movestogo") {
                        ss >> nextTimeControl;
                    }
                }
                searchThread = std::thread(go);
            } else if (cmd == "stop") {
                stopSearch = true;
                if (searchThread.joinable())
                    searchThread.join();
            } else if (cmd == "quit") {
                stopSearch = true;
                if (searchThread.joinable())
                    searchThread.join();
                break;
            } else if (cmd == "display" || cmd == "d") {
                b.displayBoard();
            } else {
                logMessage("Invalid command!");
            }
        }
    }
}