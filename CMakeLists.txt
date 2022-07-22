cmake_minimum_required(VERSION 3.22)
project(Wire)

SET(CMAKE_CXX_FLAGS "-Ofast -pthread")
set(CMAKE_CXX_STANDARD 20)

add_executable(Wire src/main.cpp src/piece.h src/board.cpp src/board.h src/movegen.cpp src/movegen.h src/move.cpp src/move.h src/utils.cpp src/utils.h src/constants.h src/bitboard.cpp src/bitboard.h src/eval.cpp src/eval.h src/search.cpp src/search.h src/globals.h src/move_ordering.cpp src/move_ordering.h src/transposition_table.cpp src/transposition_table.h src/uci.cpp src/uci.h src/bench.cpp src/bench.h)
