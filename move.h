// main source is https://www.chessprogramming.org/Encoding_Moves
#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H
#include <iostream>

const unsigned int PROMO_FLAG = 0x8;    // 0b1000
const unsigned int CAPTURE_FLAG = 0x4;  // 0b0100
const unsigned int SPECIAL1_FLAG = 0x2; // 0b0010
const unsigned int SPECIAL2_FLAG = 0x1; // 0b0001
class move {
public:
    move(unsigned int, unsigned int, unsigned int, unsigned char);

    move(unsigned int, unsigned int, unsigned int);

    move();

    void operator=(move);

    bool operator<(move) const;

    unsigned int getTo() const;

    unsigned int getFrom() const;

    unsigned char getCapturedPiece() const;

    bool isFlag(unsigned int) const;

    bool operator==(move) const;

    bool operator!=(move) const;

    bool isCapture() const;

    bool isPromo() const;

    bool isSpecial1() const;

    bool isSpecial2() const;

    bool isQuiet() const;

    explicit operator bool() const;

    bool isNULL() const;

    std::string str() const;

private:
    unsigned int data;
};

std::ostream& operator<<(std::ostream&, const move&);

#endif //CHESS_MOVE_H
