//
// Created by szil on 2022.07.06..
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H


#include "constants.h"

class bitboard {
public:
    U64 bb;

    bitboard();

    bitboard(U64);

    int pop_cnt();

    bool get(unsigned int) const;

    void set(unsigned int);

    void clear(unsigned int);

    void toggle(unsigned int);

    void display();

    bool operator==(const bitboard &) const;

    bool operator!=(const bitboard &) const;

    bitboard operator&(const bitboard &) const;

    bitboard operator|(const bitboard &) const;

    bitboard operator^(const bitboard &) const;

    bitboard operator~() const;

    bitboard operator<<(unsigned int) const;

    bitboard operator>>(unsigned int) const;

    bitboard operator-(const bitboard &) const;

    bitboard operator+(const bitboard &) const;

    bitboard operator*(const bitboard &) const;

    void operator&=(const bitboard &);

    void operator|=(const bitboard &);

    void operator^=(const bitboard &);

    void operator<<=(unsigned int);

    void operator>>=(unsigned int);

    bool operator[](unsigned int) const;

    explicit operator bool() const;

    explicit operator unsigned int() const;

    explicit operator U64() const;

    unsigned int lsb() const;

    unsigned int pop_lsb();

    bitboard reverse();

    bitboard step(Direction dir);
};


#endif //CHESS_BITBOARD_H
