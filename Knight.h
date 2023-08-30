#ifndef KNIGHT_H
#define KNIGHT_H

#include "Position.h"

#define MAX_KNIGHT_MOVES 8

Position* getKnightMoves(char game[8][8], Position startPos, int* count);

#endif // KNIGHT_H