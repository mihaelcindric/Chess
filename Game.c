#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"

#include "SDL.h"
#include "SDL_image.h"

#include "Game.h"
#include "Movement.h"


bool isCheck(char game[8][8], int turn, int attackedFields[8][8]) {
    if (turn == 1) {    // white turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (game[i][j] == 'K') {
                    if (attackedFields[i][j] == 1) {
                        return true;
                    }
                    return false;
                }
            }
        }
    }
    else {  // black turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (game[i][j] == 'k') {
                    if (attackedFields[i][j] == 1) {
                        return true;
                    }
                    return false;
                }
            }
        }
    }

    return false;
}


bool isCheckmate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove) {
    Position startPos = { 0, 0 };
    int moveCount = 0;

    if (turn == 1) {    // white turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                startPos.row = i;
                startPos.col = j;

                if (isupper(game[i][j]) && getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                    if (moveCount > 0)
                        return false;
                }
            }
        }
    }
    else {  // black turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                startPos.row = i;
                startPos.col = j;

                if (islower(game[i][j]) && getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                    if (moveCount > 0)
                        return false;
                }
            }
        }
    }

    return true;
}

bool isStalemate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove) {
    Position startPos = { 0, 0 };
    int moveCount = 0;

    if (turn == 1) {    // white turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                startPos.row = i;
                startPos.col = j;

                if (isupper(game[i][j]) && getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                    if (moveCount > 0)
                        return false;
                }
            }
        }
    }
    else {  // black turn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                startPos.row = i;
                startPos.col = j;

                if (islower(game[i][j]) && getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                    if (moveCount > 0)
                        return false;
                }
            }
        }
    }

    return true;
}


bool isThreefoldRepetition(const char* lastBoardState) {
    FILE* historyFile = fopen("data/game_history.txt", "r");
    if (historyFile == NULL) {
        perror("Could not open the file");
        return false;
    }

    int counter = 0;
    char line[100];
    char lastBoard[80], lastEnPassant[3], lastCastling[5];

    char previousState[90];
    strcpy(previousState, lastBoardState);

    // Koristimo tokene da izdvojimo bitne dijelove iz lastBoardState
    char* token = strtok(previousState, " ");
    strcpy(lastBoard, token);
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    strcpy(lastCastling, token);
    token = strtok(NULL, " ");
    strcpy(lastEnPassant, token);

    while (fgets(line, sizeof(line), historyFile) != NULL) {
        char boardFromFile[80], enPassantFromFile[3], castlingFromFile[5];

        // Koristimo tokene da izdvojimo bitne dijelove iz svake linije
        token = strtok(line, " ");
        strcpy(boardFromFile, token);
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        strcpy(castlingFromFile, token);
        token = strtok(NULL, " ");
        strcpy(enPassantFromFile, token);

        if (strcmp(lastBoard, boardFromFile) == 0 &&
            strcmp(lastEnPassant, enPassantFromFile) == 0 &&
            strcmp(lastCastling, castlingFromFile) == 0) {
            counter++;
        }
    }

    fclose(historyFile);

    return (counter >= 3);
}


bool is50MoveRule(int stallingMoves) {
    if (stallingMoves == 100)
        return true;
    return false;
}


void endGame() {
    // TODO
}

void drawGame() {
    // TODO
}