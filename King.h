#ifndef KING_H
#define KING_H

#include "Position.h"

#define MAX_KING_MOVES 8

Position* getKingMoves(char game[8][8], Position startPos, int* count, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8]);
Position* castling(char game[8][8], char piece, Position clickedPos, bool hasKingMoved[2], bool hasRookMoved[4]);

#endif // KING_H