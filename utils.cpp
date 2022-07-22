#include <cassert>
#include "utils.h"
#include "movegen.h"

unsigned int indexToRank(unsigned int index) {
    return index >> 3;
}

unsigned int indexToFile(unsigned int index) {
    return index & 7;
}

unsigned int SqToIndex(unsigned int rank, unsigned int file) {
    return (rank << 3) + file;
}

unsigned char encodePiece(Color color, Piece piece) {
    return (color << 3) | piece;
}


Color charToColor(char c) {
    if (c >= 'A' && c <= 'Z') return WHITE;
    else return BLACK;
}

Piece charToPiece(char c) {
    if (charToColor(c) == WHITE) c += 32;
    switch (c) {
        case 'p':
            return PAWN;
        case 'r':
            return ROOK;
        case 'n':
            return KNIGHT;
        case 'b':
            return BISHOP;
        case 'q':
            return QUEEN;
        case 'k':
            return KING;
        default:
            return PIECE_EMPTY;
    }
}

char pieceToChar(Color color, Piece piece) { // I understand the naming inconsistency
    char base;
    switch (piece) {
        case PAWN:
            base = 'p';
            break;
        case ROOK:
            base = 'r';
            break;
        case KNIGHT:
            base = 'n';
            break;
        case BISHOP:
            base = 'b';
            break;
        case QUEEN:
            base = 'q';
            break;
        case KING:
            base = 'k';
            break;
        case PIECE_EMPTY:
            base = ' ';
            break;
    }
    if (color == WHITE) base -= 32;
    return base;
}

std::string formatIndex(unsigned int index) {
    return std::string() + (char) ('a' + (char) index % 8) + (char) ('1' + (char) (index / 8));
}

std::pair<Color, Piece> decodePiece(unsigned char c) {
    return {c >> 3 ? BLACK : WHITE, PIECE_TABLE[c & 7]};
}

move stringToMove(std::string s) {
    unsigned int start = (s[0] - 'a') + (s[1] - '1') * 8;
    unsigned int end = (s[2] - 'a') + (s[3] - '1') * 8;
    move moves[200];
    unsigned int moveCount = movegen::generate_moves(moves, false);
    move m;
    for (int i = 0; i < moveCount; i++) {
        if (moves[i].getFrom() == start && moves[i].getTo() == end) {
            m = moves[i];
        }
    }
    assert(!m.isNULL());
    return m;
}