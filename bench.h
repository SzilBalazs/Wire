#ifndef WIRE_BENCH_H
#define WIRE_BENCH_H


#include <string>
#include "constants.h"

namespace bench {

	void init();

	void perftTest();

    void searchTest(bool);

	U64 perft(int depth);

	U64 perftPosition(std::string fen, int depth);
};


#endif //WIRE_BENCH_H
