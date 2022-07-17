cmake_minimum_required(VERSION 3.22)
project(Wire)

SET(CMAKE_CXX_FLAGS "-Ofast -pthread")
set(CMAKE_CXX_STANDARD 14)

add_executable(Wire main.cpp piece.h board.cpp board.h movegen.cpp movegen.h move.cpp move.h utils.cpp utils.h constants.h bitboard.cpp bitboard.h eval.cpp eval.h search.cpp search.h globals.h move_ordering.cpp move_ordering.h transposition_table.cpp transposition_table.h uci.cpp uci.h)
