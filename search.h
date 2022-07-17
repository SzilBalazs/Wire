//
// Created by szil on 2022.07.09..
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "move.h"

int searchCaptures(int, int, int);
int search(unsigned int, int, int, int);
move searchRoot(unsigned int);

#endif //CHESS_SEARCH_H
