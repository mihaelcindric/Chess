#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"

#include "Movement.h"
#include "Game.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"


Position* getPossibleMoves(char game[8][8], int turn, Position startPos, int* moveCount, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove) {
    char selectedPiece = game[startPos.row][startPos.col];
    Position* validMoves = NULL;

    switch (tolower(selectedPiece)) {
    case 'p':
        validMoves = getPawnMoves(game, startPos, moveCount, lastMove);
        break;
    case 'r':
        validMoves = getRookMoves(game, startPos, moveCount);
        break;
    case 'n':
        validMoves = getKnightMoves(game, startPos, moveCount);
        break;
    case 'b':
        validMoves = getBishopMoves(game, startPos, moveCount);
        break;
    case 'q':
        validMoves = getQueenMoves(game, startPos, moveCount);
        break;
    case 'k':
        validMoves = getKingMoves(game, startPos, moveCount, hasKingMoved, hasRookMoved, attackedFields);
        break;
    }

    // remove any moves that lead to check/check-mate
    filterLegalMoves(game, validMoves, moveCount, startPos, turn, hasKingMoved, hasRookMoved, lastMove);

    return validMoves;
}


void updateAttackedFields(char game[8][8], int attackedFields[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove) {
    // Resetiranje polja
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            attackedFields[i][j] = 0;
        }
    }

    // Prolazak kroz sve figure odredene boje
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (turn == 1 && isupper(game[row][col]) || turn == -1 && islower(game[row][col])) {
                Position startPos = { row, col };
                Position* moves = NULL;
                int count;

                switch (toupper(game[row][col])) {
                case 'P':
                    moves = getPawnMoves(game, startPos, &count, lastMove);
                    break;
                case 'R':
                    moves = getRookMoves(game, startPos, &count);
                    break;
                case 'N':
                    moves = getKnightMoves(game, startPos, &count);
                    break;
                case 'B':
                    moves = getBishopMoves(game, startPos, &count);
                    break;
                case 'Q':
                    moves = getQueenMoves(game, startPos, &count);
                    break;
                case 'K':
                    moves = getKingMoves(game, startPos, &count, hasKingMoved, hasRookMoved, attackedFields);
                    break;
                }

                for (int i = 0; i < count; i++) {
                    // Removing forward moves from attacking movements of a pawn
                    if (tolower(game[row][col]) == 'p' && moves[i].col == startPos.col)
                        continue;

                    attackedFields[moves[i].row][moves[i].col] = 1;
                }

                free(moves); // Oslobaðanje memorije
            }
        }
    }
}


void filterLegalMoves(char game[8][8], Position* moves, int* moveCount, Position startPos, int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove) {
    int attackedFields[8][8] = { 0 };

    int writeIdx = 0;
    for (int readIdx = 0; readIdx < *moveCount; readIdx++) {
        char test[8][8];
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                test[i][j] = game[i][j];
            }
        }

        test[moves[readIdx].row][moves[readIdx].col] = test[startPos.row][startPos.col];
        test[startPos.row][startPos.col] = ' ';

        updateAttackedFields(test, attackedFields, turn * -1, hasKingMoved, hasRookMoved, lastMove);

        if (!isCheck(test, turn, attackedFields)) {
            moves[writeIdx] = moves[readIdx];
            writeIdx++;
        }
    }

    *moveCount = writeIdx;
}


char* isCastlingPossible(char piece, char game[8][8], bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove) {
    static bool possibleCastling[4]; // static kako bi se saèuvala vrijednost izmeðu poziva

    // Resetiraj niz na false
    for (int i = 0; i < 4; i++) {
        possibleCastling[i] = false;
    }

    int whiteAttackedFields[8][8] = { 0 };
    int blackAttackedFields[8][8] = { 0 };
    updateAttackedFields(game, whiteAttackedFields, 1, hasKingMoved, hasRookMoved, lastMove);
    updateAttackedFields(game, blackAttackedFields, -1, hasKingMoved, hasRookMoved, lastMove);

    for (int i = 0; i < 2; i++) {
        int attackedFields[8][8] = { 0 };

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (i == 0)
                    attackedFields[row][col] = blackAttackedFields[row][col];
                else
                    attackedFields[row][col] = whiteAttackedFields[row][col];
            }
        }

        if (!hasKingMoved[i]) {
            // Provjera lijeva (damska) rokada
            if (!hasRookMoved[i * 2] && game[i * 7][1] == ' ' && game[i * 7][2] == ' ' && game[i * 7][3] == ' ' &&
                !attackedFields[i * 7][1] == 0 && !attackedFields[i * 7][2] == 0 && !attackedFields[i * 7][3] == 0) {
                possibleCastling[i * 2] = true;
            }

            // Provjera desna (kraljeva) rokada
            if (!hasRookMoved[i * 2 + 1] && game[i * 7][5] == ' ' && game[i * 7][6] == ' ' &&
                !attackedFields[i * 7][5] == 0 && !attackedFields[i * 7][6] == 0) {
                possibleCastling[i * 2 + 1] = true;
            }
        }
    }

    char castlingFEN[5] = "-\0";

    if (possibleCastling[0] || possibleCastling[1] || possibleCastling[2] || possibleCastling[3]) {
        strcpy(castlingFEN, "");
    }
    if (possibleCastling[0]) {
        strcat(castlingFEN, "q");
    }
    if (possibleCastling[1]) {
        strcat(castlingFEN, "k");
    }
    if (possibleCastling[2]) {
        strcat(castlingFEN, "Q");
    }
    if (possibleCastling[3]) {
        strcat(castlingFEN, "K");
    }


    return castlingFEN;
}
