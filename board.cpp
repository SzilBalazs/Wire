#include <cassert>
#include <random>
#include "board.h"
#include "constants.h"

U64 pieceRandTable[64][2][6]; // sq color piece
U64 epRandTable[64];
U64 castleRandTable[4];
U64 sideRandTable[2];

void hashInit() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<U64> dist(0, (1ULL << 63) + 1);

    for (unsigned int sq = 0; sq < 64; sq++) {
        for (unsigned int color = 0; color < 2; color++) {
            for (unsigned int piece = 0; piece < 6; piece++) {
                pieceRandTable[sq][color][piece] = dist(mt);
            }
        }
        epRandTable[sq] = dist(mt);
    }

    for (U64 &i : castleRandTable) {
        i = dist(mt);
    }

    for (U64 &i : sideRandTable) {
        i = dist(mt);
    }
}

bool board::canCastle(unsigned char mask) const {
    return castle_rights.top() & mask;
}

void board::removeCastleRight(unsigned char mask) {
    castle_rights.top() = ~mask & castle_rights.top();
}

void board::updateHash() {
    if (ep.top() != -1) hash ^= epRandTable[ep.top()];
    if (canCastle(WK_MASK)) hash ^= castleRandTable[0];
    if (canCastle(WQ_MASK)) hash ^= castleRandTable[1];
    if (canCastle(BK_MASK)) hash ^= castleRandTable[2];
    if (canCastle(BQ_MASK)) hash ^= castleRandTable[3];
    if (stm == WHITE) hash ^= sideRandTable[0];
    else hash ^= sideRandTable[1];
}

void board::setSquare(Color color, Piece piece, unsigned int index) {
    if (colors[index] != COLOR_EMPTY) clearSquare(index);

    colors[index] = color;
    pieces[index] = piece;
    pieceBB[piece].set(index);
    allPieceBB[color].set(index);

    if (piece == KING) {
        if (color == WHITE) wking = index;
        else bking = index;
    }

    hash ^= pieceRandTable[index][color][piece];
}

void board::clearSquare(unsigned int index) {
    if (colors[index] != COLOR_EMPTY) {
        hash ^= pieceRandTable[index][colors[index]][pieces[index]];
    }

    for (unsigned int i = 0; i < 6; i++) {
        pieceBB[i].clear(index);
    }

    colors[index] = COLOR_EMPTY;
    pieces[index] = PIECE_EMPTY;
    allPieceBB[0].clear(index);
    allPieceBB[1].clear(index);
}

void board::clearPosition() {
    status = PLAYING;
    stm = WHITE;
    ep = std::stack<int>();
    castle_rights = std::stack<unsigned char>();
    hashHistory = std::vector<U64>();
    lastIrreversibleMove = std::stack<unsigned int>();
    lastIrreversibleMove.push(0);
    ep.push(-1);
    castle_rights.push(0);
    wking = 64;
    bking = 64;
    for (int ind = 0; ind < 64; ind++) {
        clearSquare(ind);
    }
    hash = 0;
}

void board::loadPositionFromFEN(std::string fen) { // NOT CHECKING FOR VALIDITY -> TODO check for validity
    clearPosition();
    unsigned int index = 56, ind = 0;
    for (; fen[ind] != ' ' && ind < fen.size(); ind++) {
        char c = fen[ind];
        if (c >= '1' && c <= '8') {
            index += c - '0';
        } else if (c == '/') {
            index -= 16;
        } else if (('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c)) {
            setSquare(charToColor(c), charToPiece(c), index);
            index++;
        }
    }
    ind++;
    if (fen[ind] == 'w') stm = WHITE;
    else stm = BLACK;
    ind += 2;
    for (; fen[ind] != ' ' && ind < fen.size(); ind++) {
        char c = fen[ind];
        if (c == 'K')
            castle_rights.top() |= WK_MASK;
        else if (c == 'Q')
            castle_rights.top() |= WQ_MASK;
        else if (c == 'k')
            castle_rights.top() |= BK_MASK;
        else if (c == 'q')
            castle_rights.top() |= BQ_MASK;
    }
    ind++;
    if (fen[ind] != '-') {
        ep.top() = (fen[ind] - 'a') + (fen[ind + 1] - '1') * 8;
        ind += 3;
    } else {
        ind += 2;
        ep.top() = -1;
    }
    hashHistory.emplace_back(getHash());
    // TODO read halfmove counter and fullmove counter
}

void board::displayBoard() {
    std::cout << "\n\n    a b c d e f g h\n"
                 "   ----------------";
    for (int i = 56; i >= 0; i++) {
        if (i % 8 == 0) {
            std::cout << std::endl << i / 8 + 1 << " | ";
        }
        std::cout << pieceToChar(colors[i], pieces[i]) << " ";
        if (i % 8 == 7) {
            i -= 16;
        }
    }
    std::cout << std::endl;
}

void board::movePiece(unsigned int from, unsigned int to) { // This is faster, use this if you can
    setSquare(colors[from], pieces[from], to);
    clearSquare(from);
}

void board::movePiece(unsigned int from, unsigned int to, Piece p) {
    setSquare(stm, p, to);
    clearSquare(from);
}

void board::movePiece(unsigned int from, unsigned int to, Piece p, Color c) {
    setSquare(c, p, to);
    clearSquare(from);
}

void board::makeMove(move m) {
    updateHash(); // we remove castle rights, ep, and stm from the hash
    ep.push(-1);
    castle_rights.push(castle_rights.top());

    Color op = (stm == WHITE ? BLACK : WHITE);

    bool a = m.isPromo(), b = m.isCapture(), c = m.isSpecial1(), d = m.isSpecial2();

    // update last irreversible move
    if (b || pieceBB[PAWN].get(m.getFrom()))
        lastIrreversibleMove.push(hashHistory.size());
    else
        lastIrreversibleMove.push(0);

    if (a) { // Promotion (capture only makes a diff in undoMove)
        Piece p;
        if (c && d) p = QUEEN;
        else if (c && !d) p = ROOK;
        else if (!c && d) p = BISHOP;
        else p = KNIGHT;
        movePiece(m.getFrom(), m.getTo(), p);
    } else if (b && !c && d) { // en passant
        movePiece(m.getFrom(), m.getTo());
        clearSquare(m.getTo() + (stm == WHITE ? -8 : 8));
    } else if (!b && !c && d) { // double pawn push
        movePiece(m.getFrom(), m.getTo());
        ep.top() = m.getTo() + (stm == WHITE ? -8 : 8);
    } else if (!b && c && !d) {
        if (stm == WHITE) {
            movePiece(4, 6); // king
            movePiece(7, 5); // rook
            removeCastleRight(WK_MASK);
            removeCastleRight(WQ_MASK);
        } else {
            movePiece(60, 62); // king
            movePiece(63, 61); // rook
            removeCastleRight(BK_MASK);
            removeCastleRight(BQ_MASK);
        }
    } else if (!b && c && d) {
        if (stm == WHITE) {
            movePiece(4, 2); // king
            movePiece(0, 3); // rook
            removeCastleRight(WK_MASK);
            removeCastleRight(WQ_MASK);
        } else {
            movePiece(60, 58); // king
            movePiece(56, 59); // rook
            removeCastleRight(BK_MASK);
            removeCastleRight(BQ_MASK);
        }
    } else {
        movePiece(m.getFrom(), m.getTo());
    }
    if (m.getFrom() == 0 || m.getTo() == 0) removeCastleRight(WQ_MASK);
    else if (m.getFrom() == 7 || m.getTo() == 7) removeCastleRight(WK_MASK);
    else if (m.getFrom() == 56 || m.getTo() == 56) removeCastleRight(BQ_MASK);
    else if (m.getFrom() == 63 || m.getTo() == 63) removeCastleRight(BK_MASK);
    if (m.getFrom() == 4) {
        removeCastleRight(WQ_MASK);
        removeCastleRight(WK_MASK);
    } else if (m.getFrom() == 60) {
        removeCastleRight(BQ_MASK);
        removeCastleRight(BK_MASK);
    }
    moves.push(m);
    stm = op;

    // check for repetition
    unsigned int counter = 1;
    for (U64 i : hashHistory) { // TODO speed this up
        if (i == hash) {
            counter++;
        }
    }
    if (counter >= 3) {
        status = DRAW;
    }

    hashHistory.emplace_back(getHash());
    updateHash(); // we only add them back after repetition check because they don't have to be the same
}

void board::undoMove() {
    updateHash();
    move m = moves.top();
    Piece p = decodePiece(m.getCapturedPiece()).second;
    moves.pop();
    ep.pop();
    castle_rights.pop();

    Color op = (stm == WHITE ? BLACK : WHITE);

    bool a = m.isPromo(), b = m.isCapture(), c = m.isSpecial1(), d = m.isSpecial2();
    if (a) { // Promotion
        movePiece(m.getTo(), m.getFrom(), PAWN, op);
    } else if (b && !c && d) { // En passant
        movePiece(m.getTo(), m.getFrom());
        setSquare(stm, PAWN, m.getTo() + (op == WHITE ? -8 : 8));
    } else if (!b && c && !d) {
        if (op == WHITE) {
            movePiece(6, 4); // king
            movePiece(5, 7); // rook
        } else {
            movePiece(62, 60); // king
            movePiece(61, 63); // rook
        }
    } else if (!b && c && d) {
        if (op == WHITE) {
            movePiece(2, 4); // king
            movePiece(3, 0); // rook
        } else {
            movePiece(58, 60); // king
            movePiece(59, 56); // rook
        }
    } else {
        movePiece(m.getTo(), m.getFrom());
    }


    if (b && !(!a && !c && d)) // if it was a capture (and not en passant) we restore the piece
        setSquare(stm, p, m.getTo());

    stm = op;
    status = PLAYING;
    updateHash();
    hashHistory.pop_back();
    lastIrreversibleMove.pop();
}

U64 board::getHash() const {
    return hash;
}
