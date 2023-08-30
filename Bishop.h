#ifndef BISHOP_H
#define BISHOP_H

#include "Position.h"

#define MAX_BISHOP_MOVES 13

Position* getBishopMoves(char game[8][8], Position startPos, int* count);

#endif // BISHOP_H