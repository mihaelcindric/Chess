#ifndef GAME_H
#define GAME_H

#include "Position.h"

#include "SDL.h"
#include "SDL_image.h"

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 680
#define BOARD_WIDTH 640
#define BOARD_HEIGHT 640
#define TEXT_WIDHT 240
#define TEXT_HEIGHT 80

bool isCheck(char game[8][8], int turn, int attackedFields[8][8]);
bool isCheckmate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
bool isStalemate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
bool isThreefoldRepetition(const char* lastBoardState);
bool is50MoveRule(int stallingMoves);
void endGame(SDL_Renderer* renderer);
void drawGame(SDL_Renderer* renderer);

#endif // GAME_H