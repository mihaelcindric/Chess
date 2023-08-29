#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "stdbool.h"

#include "Bishop.h"

Position* getBishopMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_BISHOP_MOVES * sizeof(Position));
    *count = 0;

    char bishop = game[startPos.row][startPos.col];

    // Definiramo smjerove kretanja lovca: gore-lijevo, gore-desno, dolje-lijevo, dolje-desno
    int rowMoves[4] = { -1, -1, 1, 1 };
    int colMoves[4] = { -1, 1, -1, 1 };

    for (int dir = 0; dir < 4; dir++) {
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
                else if ((isupper(bishop) && islower(game[newRow][newCol])) ||
                    (islower(bishop) && isupper(game[newRow][newCol]))) {
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
