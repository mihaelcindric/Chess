#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

#include "Rook.h"


Position* getRookMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_ROOK_MOVES * sizeof(Position));
    *count = 0;

    char rook = game[startPos.row][startPos.col];

    // Provjera vodoravno prema desno
    for (int j = startPos.col + 1; j < 8; j++) {
        if (game[startPos.row][j] == ' ') {
            moves[*count].row = startPos.row;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startPos.row][j])) || (islower(rook) && isupper(game[startPos.row][j]))) {
                moves[*count].row = startPos.row;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera vodoravno prema lijevo
    for (int j = startPos.col - 1; j >= 0; j--) {
        if (game[startPos.row][j] == ' ') {
            moves[*count].row = startPos.row;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startPos.row][j])) || (islower(rook) && isupper(game[startPos.row][j]))) {
                moves[*count].row = startPos.row;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema gore
    for (int i = startPos.row - 1; i >= 0; i--) {
        if (game[i][startPos.col] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startPos.col;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startPos.col])) || (islower(rook) && isupper(game[i][startPos.col]))) {
                moves[*count].row = i;
                moves[*count].col = startPos.col;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema dolje
    for (int i = startPos.row + 1; i < 8; i++) {
        if (game[i][startPos.col] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startPos.col;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startPos.col])) || (islower(rook) && isupper(game[i][startPos.col]))) {
                moves[*count].row = i;
                moves[*count].col = startPos.col;
                (*count)++;
            }
            break;
        }
    }

    return moves;
}