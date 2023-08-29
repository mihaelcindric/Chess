#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

#include "Knight.h"


Position* getKnightMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_KNIGHT_MOVES * sizeof(Position));
    *count = 0;

    char knight = game[startPos.row][startPos.col];

    // Definiramo sve moguæe poteze konja
    int rowMoves[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int colMoves[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

    for (int i = 0; i < 8; i++) {
        int newRow = startPos.row + rowMoves[i];
        int newCol = startPos.col + colMoves[i];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) { // Provjera je li unutar ploèe
            if (game[newRow][newCol] == ' ' ||
                (isupper(knight) && islower(game[newRow][newCol])) ||
                (islower(knight) && isupper(game[newRow][newCol]))) {
                moves[*count].row = newRow;
                moves[*count].col = newCol;
                (*count)++;
            }
        }
    }

    return moves;
}