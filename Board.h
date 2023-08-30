#ifndef BOARD_H

#include "SDL.h"
#include "SDL_image.h"

#include "Position.h"

void drawBoard(SDL_Renderer* renderer, char game[8][8]);
void updateBoard(SDL_Renderer* renderer, char game[8][8], Position oldPos, Position newPos);
void drawPiece(SDL_Renderer* renderer, char piece, int row, int col);
void showPossibleMoves(SDL_Renderer* renderer, char game[8][8], Position possibleMoves[], int count);
void showCurrentlyChosen(SDL_Renderer* renderer, char game[8][8], Position chosenPosition);
void resetBoardFields(SDL_Renderer* renderer, char game[8][8], int turn, Position startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
void resetBoardCenter(SDL_Renderer* renderer, char game[8][8]);

#endif // BOARD_H