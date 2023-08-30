#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "stdbool.h"

#include "Queen.h"


Position* getQueenMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_QUEEN_MOVES * sizeof(Position));
    *count = 0;

    char queen = game[startPos.row][startPos.col];

    // Definiramo smjerove kretanja kraljice: horizontalno, vertikalno i dijagonalno
    int rowMoves[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    int colMoves[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };

    for (int dir = 0; dir < 8; dir++) {
        int step = 1;

        while (true) {
            int newRow = startPos.row + step * rowMoves[dir];
            int newCol = startPos.col + step * colMoves[dir];

            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                // Ako je unutar ploèe

                if (game[newRow][newCol] == ' ') {
                    moves[*count].row = newRow;
                    moves[*count].col = newCol;
                    (*count)++;
                }
                else if ((isupper(queen) && islower(game[newRow][newCol])) ||
                    (islower(queen) && isupper(game[newRow][newCol]))) {
                    // Ako se može pojesti suprotna figura
                    moves[*count].row = newRow;
                    moves[*count].col = newCol;
                    (*count)++;
                    break; // Ne možemo se kretati dalje preko figura
                }
                else {
                    break; // Blokiran je vlastitom figurom
                }

                step++; // Pomièemo se dalje u trenutnom smjeru
            }
            else {
                break; // Izvan ploèe
            }
        }
    }

    return moves;
}