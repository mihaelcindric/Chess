#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "SDL.h"
#include "SDL_image.h"

#include "King.h"


Position* getKingMoves(char game[8][8], Position startPos, int* count, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8]) {
    Position* moves = (Position*)malloc(MAX_KING_MOVES * sizeof(Position));
    *count = 0;

    char king = game[startPos.row][startPos.col];

    // Definiramo smjerove kretanja kralja: horizontalno, vertikalno i dijagonalno
    int rowMoves[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    int colMoves[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };

    for (int dir = 0; dir < 8; dir++) {
        int newRow = startPos.row + rowMoves[dir];
        int newCol = startPos.col + colMoves[dir];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            // Ako je unutar ploèe

            if (game[newRow][newCol] == ' ' && attackedFields[newRow][newCol] == 0) {
                moves[*count].row = newRow;
                moves[*count].col = newCol;
                (*count)++;
            }
            else if ((isupper(king) && islower(game[newRow][newCol])) ||
                (islower(king) && isupper(game[newRow][newCol]))) {
                // Ako se može pojesti suprotna figura
                moves[*count].row = newRow;
                moves[*count].col = newCol;
                (*count)++;
            }
            // Kralj ne može preæi preko drugih figura, pa nema potrebe za daljnjom provjerom
        }
    }

    // Rokada crni
    if (king == 'k' && !hasKingMoved[0]) {
        int kingRow = startPos.row;

        // Damska rokada (top na daminoj strani)
        if (!hasRookMoved[1] && game[kingRow][1] == ' ' && game[kingRow][2] == ' ' && game[kingRow][3] == ' ' &&
            attackedFields[kingRow][1] == 0 && attackedFields[kingRow][2] == 0 && attackedFields[kingRow][3] == 0 /* i provjera napada */) {
            moves[*count].row = kingRow;
            moves[*count].col = 2;
            (*count)++;
        }

        // Kraljeva rokada (top na kraljevoj strani)
        if (!hasRookMoved[0] && game[kingRow][5] == ' ' && game[kingRow][6] == ' ' &&
            attackedFields[kingRow][5] == 0 && attackedFields[kingRow][6] == 0 /* i provjera napada */) {
            moves[*count].row = kingRow;
            moves[*count].col = 6;
            (*count)++;
        }
    }
    else if (king == 'K' && !hasKingMoved[1]) {    // rokada bijeli
        int kingRow = startPos.row;

        // Damska rokada (top na daminoj strani)
        if (!hasRookMoved[3] && game[kingRow][1] == ' ' && game[kingRow][2] == ' ' && game[kingRow][3] == ' ' &&
            attackedFields[kingRow][1] == 0 && attackedFields[kingRow][2] == 0 && attackedFields[kingRow][3] == 0 /* i provjera napada */) {
            moves[*count].row = kingRow;
            moves[*count].col = 2;
            (*count)++;
        }

        // Kraljeva rokada (top na kraljevoj strani)
        if (!hasRookMoved[2] && game[kingRow][5] == ' ' && game[kingRow][6] == ' ' &&
            attackedFields[kingRow][5] == 0 && attackedFields[kingRow][6] == 0 /* i provjera napada */) {
            moves[*count].row = kingRow;
            moves[*count].col = 6;
            (*count)++;
        }
    }


    return moves;
}


Position* castling(char game[8][8], char piece, Position clickedPos, bool hasKingMoved[2], bool hasRookMoved[4]) {
    // castling
    if (piece == 'k') { // black
        if (hasKingMoved[0] == false && clickedPos.row == 0) {
            if (clickedPos.col == 2 && hasRookMoved[0] == false) {
                game[0][0] = ' ';
                game[0][3] = 'r';
                hasRookMoved[0] = true;
                Position rookPosOld = { 0, 0 };
                Position rookPosNew = { 0, 3 };
                Position fields[2] = { rookPosOld, rookPosNew };
                return fields;
            }
            else if (clickedPos.col == 6 && hasRookMoved[1] == false) {
                game[0][7] = ' ';
                game[0][5] = 'r';
                hasRookMoved[1] = true;
                Position rookPosOld = { 0, 7 };
                Position rookPosNew = { 0, 5 };
                Position fields[2] = { rookPosOld, rookPosNew };
                return fields;
            }
        }
        hasKingMoved[0] = true;
    }
    else if (piece == 'K') {    // white
        if (hasKingMoved[1] == false && clickedPos.row == 7) {
            if (clickedPos.col == 2 && hasRookMoved[2] == false) {
                game[7][0] = ' ';
                game[7][3] = 'R';
                hasRookMoved[2] = true;
                Position rookPosOld = { 7, 0 };
                Position rookPosNew = { 7, 3 };
                Position fields[2] = { rookPosOld, rookPosNew };
                return fields;
            }
            else if (clickedPos.col == 6 && hasRookMoved[3] == false) {
                game[7][7] = ' ';
                game[7][5] = 'R';
                hasRookMoved[3] = true;
                Position rookPosOld = { 7, 7 };
                Position rookPosNew = { 7, 5 };
                Position fields[2] = { rookPosOld, rookPosNew };
                return fields;
            }
        }
        hasKingMoved[1] = true;
    }

    return NULL;
}