#ifndef QUEEN_H
#define QUEEN_H

#include "Position.h"

#define MAX_QUEEN_MOVES 27

Position* getQueenMoves(char game[8][8], Position startPos, int* count);

#endif // QUEEN_H