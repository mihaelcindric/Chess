#ifndef ROOK_H
#define ROOK_H

#include "Position.h"

#define MAX_ROOK_MOVES 14

Position* getRookMoves(char game[8][8], Position startPos, int* count);

#endif // ROOK_H
