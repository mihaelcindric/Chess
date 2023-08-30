#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"

#include "SDL.h"
#include "SDL_image.h"

#include "Board.h"
#include "Movement.h"


void drawBoard(SDL_Renderer* renderer, char game[8][8]) {
    int offset = 20;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            SDL_Rect rect = { i * 80 + offset, j * 80 + offset, 80, 80 };
            SDL_SetRenderDrawColor(renderer, (i + j) % 2 == 0 ? 139 : 233, (i + j) % 2 == 0 ? 69 : 194, (i + j) % 2 == 0 ? 19 : 166, 255);
            SDL_RenderFillRect(renderer, &rect);

            char piece = game[j][i];
            if (piece != ' ') {
                char color[6];
                strcpy(color, islower(piece) ? "black" : "white");
                char pieceName[7];
                sprintf(pieceName, "%s", (char*[]) { "rook", "knight", "bishop", "queen", "king", "pawn" }[strchr("rnbqkp", tolower(piece)) - "rnbqkp"]);
                char path[50];
                sprintf(path, "img/%s_%s.png", pieceName, color);
                SDL_Surface* surface = IMG_Load(path);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                SDL_Rect destRect = { i * 80 + offset + 20, j * 80 + offset + 10, 40, 60 };
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
                SDL_DestroyTexture(texture);
            }
        }
    }

    // Drawing the dark gray border
    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255); // Dark Gray
    SDL_Rect borderRects[4] = {
        { 0, 0, 8 * 80 + 2 * offset, offset }, // Top
        { 0, 0, offset, 8 * 80 + 2 * offset }, // Left
        { 8 * 80 + offset, 0, offset, 8 * 80 + 2 * offset }, // Right
        { 0, 8 * 80 + offset, 8 * 80 + 2 * offset, offset }  // Bottom
    };
    SDL_RenderFillRects(renderer, borderRects, 4);

    for (int i = 1; i <= 8; i++) {
        char path[50];
        sprintf(path, "img/number_%d.png", i);
        SDL_Surface* surface = IMG_Load(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect destRectLeft = { 2, (8 - i) * 80 + offset + 30, 16, 16 };
        SDL_Rect destRectRight = { 8 * 80 + offset + 2, (8 - i) * 80 + offset + 30, 16, 16 };
        SDL_RenderCopy(renderer, texture, NULL, &destRectLeft);
        SDL_RenderCopy(renderer, texture, NULL, &destRectRight);
        SDL_DestroyTexture(texture);
    }

    char letters[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    for (int i = 0; i < 8; i++) {
        char path[50];
        sprintf(path, "img/letter_%c.png", letters[i]);
        SDL_Surface* surface = IMG_Load(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect destRectTop = { i * 80 + offset + 32, offset - 18, 16, 16 };
        SDL_Rect destRectBottom = { i * 80 + offset + 32, 8 * 80 + offset, 16, 16 };
        SDL_RenderCopy(renderer, texture, NULL, &destRectTop);
        SDL_RenderCopy(renderer, texture, NULL, &destRectBottom);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}





void updateBoard(SDL_Renderer* renderer, char game[8][8], Position oldPos, Position newPos) {
    Position positions[2] = { oldPos, newPos };
    int offset = 20;

    for (int pos = 0; pos < 2; pos++) {
        int i = positions[pos].col;
        int j = positions[pos].row;

        SDL_Rect rect = { i * 80 + offset, j * 80 + offset, 80, 80 };
        if ((i + j) % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Dark Brown
        }
        else {
            SDL_SetRenderDrawColor(renderer, 233, 194, 166, 255); // Light Brown (Yellowish)
        }
        SDL_RenderFillRect(renderer, &rect);

        char piece = game[j][i];
        if (piece != ' ') {
            char color[6];
            if (islower(piece)) {
                strcpy(color, "black");
            }
            else {
                strcpy(color, "white");
            }
            piece = tolower(piece);

            char pieceName[7];
            switch (piece) {
            case 'r': strcpy(pieceName, "rook"); break;
            case 'n': strcpy(pieceName, "knight"); break;
            case 'b': strcpy(pieceName, "bishop"); break;
            case 'q': strcpy(pieceName, "queen"); break;
            case 'k': strcpy(pieceName, "king"); break;
            case 'p': strcpy(pieceName, "pawn"); break;
            default: strcpy(pieceName, ""); break;
            }

            char path[50];
            sprintf(path, "img/%s_%s.png", pieceName, color);
            SDL_Surface* surface = IMG_Load(path);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            SDL_Rect destRect = { i * 80 + 20 + offset, j * 80 + 10 + offset, 40, 60 };
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);

        }
    }
    SDL_RenderPresent(renderer);
}


void drawPiece(SDL_Renderer* renderer, char piece, int row, int col) {
    int offset = 20;

    char color[6];
    if (islower(piece)) {
        strcpy(color, "black");
    }
    else {
        strcpy(color, "white");
    }
    piece = tolower(piece);

    char pieceName[7];
    switch (piece) {
    case 'r': strcpy(pieceName, "rook"); break;
    case 'n': strcpy(pieceName, "knight"); break;
    case 'b': strcpy(pieceName, "bishop"); break;
    case 'q': strcpy(pieceName, "queen"); break;
    case 'k': strcpy(pieceName, "king"); break;
    case 'p': strcpy(pieceName, "pawn"); break;
    default: strcpy(pieceName, ""); break;
    }

    char path[50];
    sprintf(path, "img/%s_%s.png", pieceName, color);
    SDL_Surface* surface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect destRect = { col * 80 + 20 + offset, row * 80 + 10 + offset, 40, 60 };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}


void showPossibleMoves(SDL_Renderer* renderer, char game[8][8], Position possibleMoves[], int count) {
    int offset = 20;

    for (int i = 0; i < count; i++) {
        int row = possibleMoves[i].row;
        int col = possibleMoves[i].col;

        SDL_Rect rect = { col * 80 + offset, row * 80 + offset, 80, 80 };
        SDL_SetRenderDrawColor(renderer, 204, 102, 102, 255); // Crvenkasta boja za moguæe poteze
        SDL_RenderFillRect(renderer, &rect);

        // Iscrtaj figuricu ponovo ako postoji na tom polju
        char piece = game[row][col];
        if (piece != ' ') {
            drawPiece(renderer, piece, row, col);
        }
    }
    SDL_RenderPresent(renderer);
}


void showCurrentlyChosen(SDL_Renderer* renderer, char game[8][8], Position chosenPosition) {
    int offset = 20;

    int row = chosenPosition.row;
    int col = chosenPosition.col;

    SDL_Rect rect = { col * 80 + offset, row * 80 + offset, 80, 80 };
    SDL_SetRenderDrawColor(renderer, 255, 51, 51, 255); // Intenzivnija crvenkasta boja za trenutno odabrano
    SDL_RenderFillRect(renderer, &rect);

    // Iscrtaj figuricu ponovo
    char piece = game[row][col];
    drawPiece(renderer, piece, row, col);
    SDL_RenderPresent(renderer);
}


void resetBoardFields(SDL_Renderer* renderer, char game[8][8], int turn, Position startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove) {
    int offset = 20;
    int moveCount;
    Position* positions = getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove);

    // Resetiranje izgleda polja na kojemu je figura
    if ((startPos.row + startPos.col) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Dark Brown
    }
    else {
        SDL_SetRenderDrawColor(renderer, 233, 194, 166, 255); // Light Brown (Yellowish)
    }

    SDL_Rect rect = { startPos.col * 80 + offset, startPos.row * 80 + offset, 80, 80 };
    SDL_RenderFillRect(renderer, &rect);

    char piece = game[startPos.row][startPos.col];
    if (piece != ' ') {
        drawPiece(renderer, piece, startPos.row, startPos.col);
    }

    // Resetiranje ostalih polja
    for (int i = 0; i < moveCount; i++) {
        int row = positions[i].row;
        int col = positions[i].col;

        // Odabir boje ovisno o položaju polja (parna ili neparna kombinacija redaka i stupaca)
        if ((row + col) % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Dark Brown
        }
        else {
            SDL_SetRenderDrawColor(renderer, 233, 194, 166, 255); // Light Brown (Yellowish)
        }

        SDL_Rect rect = { col * 80 + offset, row * 80 + offset, 80, 80 };
        SDL_RenderFillRect(renderer, &rect);

        // Iscrtaj figuricu ponovo ako postoji na tom polju
        char piece = game[row][col];
        if (piece != ' ') {
            drawPiece(renderer, piece, row, col);
        }
    }
    SDL_RenderPresent(renderer);

    // Oslobaðanje memorije
    free(positions);
}


void resetBoardCenter(SDL_Renderer* renderer, char game[8][8]) {
    int rows[] = { 3, 4 };
    int cols[] = { 1, 2, 3, 4, 5, 6 };
    for (int i = 0; i < 6; i++) {
        Position first = { rows[0], cols[i] };
        Position second = { rows[1], cols[i] };
        updateBoard(renderer, game, first, second);
    }
}


