#include <iostream>
#include <cassert>
#include <x86gprintrin.h>
#include "bitboard.h"
#include "constants.h"


/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(U64 bb) {
    const U64 debruijn64 = 0x03f79d71b4cb0a89ULL;
    assert (bb != 0);
    return index64[((bb & -bb) * debruijn64) >> 58];
}

bitboard::bitboard() {
    bb = 0;
}

bitboard::bitboard(U64 i) {
    bb = i;
}

int bitboard::pop_cnt() {
    return __builtin_popcountll(bb);
}

bool bitboard::get(unsigned int index) const {
    return (bb >> index) & 1;
}

void bitboard::set(unsigned int index) {
    bb |= (1ULL << index);
}

void bitboard::clear(unsigned int index) {
    bb &= ~(1ULL << index);
}

void bitboard::toggle(unsigned int index) {
    bb ^= 1ULL << index;
}

void bitboard::display() {
    std::cout << "\n\n    a b c d e f g h\n"
                 "   ----------------";
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << std::endl << rank + 1 << " | ";
        for (int file = 0; file < 8; file++) {
            std::cout << get(rank * 8 + file) << " ";
        }
    }
    std::cout << std::endl << std::endl;
}

bool bitboard::operator==(const bitboard &a) const {
    return bb == a.bb;
}

bool bitboard::operator!=(const bitboard &a) const {
    return bb != a.bb;
}

bitboard bitboard::operator&(const bitboard &a) const {
    return {bb & a.bb};
}

bitboard bitboard::operator|(const bitboard &a) const {
    return {bb | a.bb};
}

bitboard bitboard::operator^(const bitboard &a) const {
    return {bb ^ a.bb};
}

bitboard bitboard::operator~() const {
    return {~bb};
}

bitboard bitboard::operator<<(const unsigned int a) const {
    return {bb << a};
}

bitboard bitboard::operator>>(const unsigned int a) const {
    return {bb >> a};
}

void bitboard::operator&=(const bitboard &a) {
    bb &= a.bb;
}

void bitboard::operator|=(const bitboard &a) {
    bb |= a.bb;
}

void bitboard::operator^=(const bitboard &a) {
    bb ^= a.bb;
}

void bitboard::operator<<=(const unsigned int a) {
    bb <<= a;
}

void bitboard::operator>>=(const unsigned int a) {
    bb >>= a;
}

bool bitboard::operator[](unsigned int index) const {
    return get(index);
}

unsigned int bitboard::lsb() const {
    return bitScanForward(bb);
}

unsigned int bitboard::pop_lsb() {
    unsigned int index = bitScanForward(bb);
    clear(index);
    return index;
}

bitboard bitboard::reverse() {
    return __bswapq(bb);
}

bitboard bitboard::step(Direction dir) {
    switch (dir) {
        case NORTH:
            return {bb << 8};
        case NoWe:
            return {(bb & notFileA) << 7};
        case WEST:
            return {(bb & notFileA) >> 1};
        case SoWe:
            return {(bb & notFileA) >> 9};
        case SOUTH:
            return {bb >> 8};
        case SoEa:
            return {(bb & notFileH) >> 7};
        case EAST:
            return {(bb & notFileH) << 1};
        case NoEa:
            return {(bb & notFileH) << 9};

    }
    return {};
}

bitboard bitboard::operator-(const bitboard &a) const {
    return {bb - a.bb};
}

bitboard bitboard::operator+(const bitboard &a) const {
    return {bb + a.bb};
}

bitboard bitboard::operator*(const bitboard &a) const {
    return {bb * a.bb};
}

bitboard::operator bool() const {
    return bb;
}

bitboard::operator unsigned int() const {
    return bb;
}

bitboard::operator U64() const {
    return bb;
}

