#include "move.h"
#include "utils.h"
#include "piece.h"

move::move(unsigned int from, unsigned int to, unsigned int flags, unsigned char capturedPiece) {
    data = ((capturedPiece & 0xf) << 16 | (flags & 0xf) << 12 | (from & 0x3f) << 6 | (to & 0x3f));
}

move::move(unsigned int from, unsigned int to, unsigned int flags) {
    data = ((COLOR_EMPTY & 0xf) << 16 | (flags & 0xf) << 12 | (from & 0x3f) << 6 | (to & 0x3f));
}

move::move() {
    data = 0;
}

bool move::isNULL() const {
    return data == 0;
}

void move::operator=(move a) {
    data = a.data;
}

bool move::operator<(move a) const {
    return data < a.data;
}

unsigned int move::getTo() const {
    return data & 0x3f;
}

unsigned int move::getFrom() const {
    return (data >> 6) & 0x3f;
}

unsigned char move::getCapturedPiece() const {
    return (data >> 16) & 0xf;
}

bool move::isFlag(unsigned int flag) const {
    return ((data >> 12) & 0xf) & flag;
}

bool move::operator==(move a) const {
    return (data & 0xFFFF) == (a.data & 0xFFFF);
}

bool move::operator!=(move a) const {
    return (data & 0xFFFF) != (a.data & 0xFFFF);
}

move::operator bool() const {
    return (data != 0);
}

bool move::isCapture() const {
    return isFlag(CAPTURE_FLAG);
}

bool move::isPromo() const {
    return isFlag(PROMO_FLAG);
}

bool move::isSpecial1() const {
    return isFlag(SPECIAL1_FLAG);
}

bool move::isSpecial2() const {
    return isFlag(SPECIAL2_FLAG);
}

bool move::isQuiet() const {
    return isFlag(0x0);
}

std::string move::str() const {
    std::string token = "";
    if (isPromo()) {
        if (!isSpecial1() && !isSpecial2()) token += "n";
        else if (isSpecial1() && !isSpecial2()) token += "b";
        else if (!isSpecial1() && isSpecial2()) token += "r";
        else token += "q";
    }
    return formatIndex(getFrom()) + formatIndex(getTo()) + token;
}

std::ostream &operator<<(std::ostream &s, const move &a) {
    return s << formatIndex(a.getFrom()) << formatIndex(a.getTo());
}





