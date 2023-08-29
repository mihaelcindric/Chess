#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Position.h"

Position* getPossibleMoves(char game[8][8], int turn, Position startPos, int* moveCount, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
void updateAttackedFields(char game[8][8], int attackedFields[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove);
void filterLegalMoves(char game[8][8], Position* moves, int* moveCount, Position startPos, int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove);
char* isCastlingPossible(char piece, char game[8][8], bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove);

#endif // MOVEMENT_H