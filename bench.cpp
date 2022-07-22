#include <chrono>
#include "bench.h"
#include "move.h"
#include "movegen.h"
#include "globals.h"
#include "search.h"

namespace bench {

	const unsigned int posCount = 8;

	std::string posFens[posCount] = {
			// Starting position
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - ",

			// Source: https://www.chessprogramming.org/Perft_Results

			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",

			"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",

			"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - ",
			"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - ",

			"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - ",

			"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - ",

            // Source end

            // Source: https://www.chess.com/games/view/16074142
            "1k4r1/2pq4/1pn5/P2b4/5P2/2PB3p/1Q4P1/1R2R1K1 w - - "
	};

	int posDepths[posCount] = {
			6,
			5,
			7,
			5,
			5,
			5,
			5,
            5
	};

	U64 posResults[posCount] = {
            119060324,
            193690690,
            178633661,
			15833292,
			15833292,
            89941194,
            164075551,
            130544847
    };

	void init() {
		hashInit();
		movegen::init();
	}

	U64 perft(int depth) {
		move moves[200];
		U64 moveCount = movegen::generate_moves(moves, false);

		// bulk-counting
		if (depth == 1) {
			return moveCount;
		}

		// dfs
		U64 nodes = 0;
		for (int i = 0 ; i < moveCount ; i++) {
			b.makeMove(moves[i]);
			nodes += perft(depth - 1);
			b.undoMove();
		}

		return nodes;
	}

	U64 perftPosition(std::string fen, int depth) {
		b.loadPositionFromFEN(fen);
		U64 nodes = perft(depth);
		return nodes;
	}

    void searchTest(bool simpleOutput) {
        if (!simpleOutput) {
            std::cout << "Starting search test...\n" << "\r[0/" << posCount << "]";
            std::cout.flush();
        }

        U64 totalNodes = 0;

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        for (int i = 0 ; i < posCount ; i++) {

            b.loadPositionFromFEN(posFens[i]);

            iterativeDeepening(posDepths[i], false);

            U64 nodes = nodeCount;

            if (!simpleOutput) {
                std::cout << "\r[" << i+1 << "/" << posCount << "] FINISHED";
                std::cout.flush();
            }

            totalNodes += nodes;
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        U64 elapsedTime =  std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

        U64 nps = totalNodes / (elapsedTime / 1000000);

        if (simpleOutput) std::cout << "Node count = " << totalNodes << "\nNode per second = " << nps*1000 << std::endl;
        else std::cout << "\n\nSearch test finished, a total of " << totalNodes << " nodes searched, with a speed of " << nps << "K nodes per second" << std::endl;


    }

	void perftTest() {
		std::cout << "Starting perft test...\n" << "\r[0/" << posCount << "]";
        std::cout.flush();

        U64 totalNodes = 0;
        std::vector<int> failedTests;

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		for (int i = 0 ; i < posCount ; i++) {
            U64 nodes = perftPosition(posFens[i], posDepths[i]);

            // Check if we failed the test
			if (nodes != posResults[i]) {
				failedTests.emplace_back(i);
                std::cout << "\r[" << i+1 << "/" << posCount << "] " << "FAILED";
			} else {
                std::cout << "\r[" << i+1 << "/" << posCount << "] " << "OK";
            }

            std::cout.flush();

            totalNodes += nodes;
		}
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        U64 elapsedTime =  std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
        U64 nps = totalNodes / (elapsedTime / 1000);


        std::cout << "\n\nPerft test finished, a total of " << totalNodes << " positions searched." << std::endl;


        // Report the results
		if (failedTests.empty()) {
			std::cout << "All positions has been found.\n\nElapsed time = " << elapsedTime << " nanoseconds\nNodes per second = " << nps << "M" << std::endl;

		} else {
            std::cout << "Failed fens:\n";
            for (int index : failedTests) {
                std::cout << index+1 << ": " << posFens[index] << "\n";
            }

            std::cout.flush();
		}
	}
}
