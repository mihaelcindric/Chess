#ifndef PAWN_H
#define PAWN_H

#include "Position.h"

#include "SDL.h"
#include "SDL_image.h"

#define MAX_PAWN_MOVES 3
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define FIELD_WIDHT 80
#define FIELD_HEIGHT 80

Position* getPawnMoves(char game[8][8], Position startPos, int* count, Position* lastMove);
bool isEnPassant(Position* lastMove, Position startPos, char piece, char game[8][8]);
void promotion(SDL_Renderer* renderer, char game[8][8], int row, int col);

#endif // PAWN_H
