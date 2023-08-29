#ifndef GAME_H
#define GAME_H

#include "Position.h"

bool isCheck(char game[8][8], int turn, int attackedFields[8][8]);
bool isCheckmate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
bool isStalemate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
bool isThreefoldRepetition(const char* lastBoardState);
bool is50MoveRule(int stallingMoves);
void endGame();
void drawGame();

#endif // GAME_H