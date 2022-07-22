#include "piece.h"
#include "constants.h"
#include "move.h"
#include <string>

#ifndef CHESS_UTILS_H
#define CHESS_UTILS_H

unsigned int indexToRank(unsigned int);

unsigned int indexToFile(unsigned int);

unsigned int SqToIndex(unsigned int, unsigned int);

std::string formatIndex(unsigned int);

unsigned char encodePiece(Color, Piece);

std::pair<Color, Piece> decodePiece(unsigned char);

Color charToColor(char);

Piece charToPiece(char);

char pieceToChar(Color, Piece);

move stringToMove(std::string);

#endif //CHESS_UTILS_H
