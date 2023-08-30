#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "stdbool.h"

#include "SDL.h"
#include "SDL_image.h"

#include "Pawn.h"


Position* getPawnMoves(char game[8][8], Position startPos, int* count, Position* lastMove) {
    Position* moves = (Position*)malloc(MAX_PAWN_MOVES * sizeof(Position));
    *count = 0;

    char pawn = game[startPos.row][startPos.col];
    int direction = islower(pawn) ? 1 : -1; // Za crne pješake ide dolje, za bijele ide gore

    // Provjera je li pijun na rubu ploèe
    if (startPos.row + direction >= 0 && startPos.row + direction < 8) {
        // Provjerite može li se pomaknuti naprijed
        if (game[startPos.row + direction][startPos.col] == ' ') {
            moves[*count].row = startPos.row + direction;
            moves[*count].col = startPos.col;
            (*count)++;
        }


        // Provjerite može li se pojesti neka figura dijagonalno
        if (startPos.col - 1 >= 0 && ((isupper(pawn) && islower(game[startPos.row + direction][startPos.col - 1])) ||
            (islower(pawn) && isupper(game[startPos.row + direction][startPos.col - 1])))) {
            moves[*count].row = startPos.row + direction;
            moves[*count].col = startPos.col - 1;
            (*count)++;
        }

        if (startPos.col + 1 < 8 && ((isupper(pawn) && islower(game[startPos.row + direction][startPos.col + 1])) ||
            (islower(pawn) && isupper(game[startPos.row + direction][startPos.col + 1])))) {
            moves[*count].row = startPos.row + direction;
            moves[*count].col = startPos.col + 1;
            (*count)++;
        }

        // Posebna pravila za poèetni pomak od dva polja za pješake
        if ((isupper(pawn) && startPos.row == 6) || (islower(pawn) && startPos.row == 1)) {
            if (game[startPos.row + direction][startPos.col] == ' ' && game[startPos.row + 2 * direction][startPos.col] == ' ') {
                moves[*count].row = startPos.row + 2 * direction;
                moves[*count].col = startPos.col;
                (*count)++;
            }
        }

        // Provjera za "en passant"
        if (isEnPassant(lastMove, startPos, game[startPos.row][startPos.col], game)) {
            int direction = islower(game[startPos.row][startPos.col]) ? 1 : -1;
            //if (startPos.col - 1 >= 0 && game[startPos.row + direction][startPos.col - 1] == ' ' || 
            //    startPos.col + 1 < 8 && game[startPos.row + direction][startPos.col + 1] == ' ') {
            moves[*count].row = startPos.row + direction;
            moves[*count].col = lastMove[1].col;
            (*count)++;
            //}
        }
    }

    return moves;
}


bool isEnPassant(Position* lastMove, Position startPos, char piece, char game[8][8]) {
    if ((piece == 'p' || piece == 'P') && abs(lastMove[1].col - startPos.col) == 1) {
        if (lastMove[1].row == startPos.row && abs(lastMove[0].row - lastMove[1].row) == 2) {
            if ((piece == 'p' && game[lastMove[1].row][lastMove[1].col] == 'P') ||
                (piece == 'P' && game[lastMove[1].row][lastMove[1].col] == 'p')) {
                return true;
            }
        }
    }
    return false;
}


void promotion(SDL_Renderer* renderer, char game[8][8], int row, int col) {
    // Koordinate i velièina gumba za promociju
    int buttonWidth = 40;
    int buttonHeight = 60;
    int startX = (BOARD_WIDTH - FIELD_WIDHT * 4) / 2;
    int startY = (BOARD_HEIGHT - FIELD_HEIGHT) / 2;

    // Okvir oko prozora
    SDL_Rect borderRect = { startX - 10, startY - 10, FIELD_WIDHT * 4 + 20, FIELD_HEIGHT + 20 };
    if (game[row][col] == 'p')
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // White border
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Black border

    SDL_RenderFillRect(renderer, &borderRect);

    // Pozadina za gumb za promociju
    SDL_Rect backgroundRect = { startX, startY, FIELD_WIDHT * 4, FIELD_HEIGHT };
    if (game[row][col] == 'p')
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Black background
    else
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // White background
    SDL_RenderFillRect(renderer, &backgroundRect);

    // Petlja za odabir promocije
    bool choosing = true;
    while (choosing) {
        // Crtanje gumba za promociju
        for (int i = 0; i < 4; i++) {
            SDL_Rect rect = { startX + i * FIELD_HEIGHT + 20, startY + 10, buttonWidth, buttonHeight };
            char* color = "";
            if (game[row][col] == 'p')
                color = "black";
            else
                color = "white";

            char* piece = "";
            switch (i) {
            case 0: piece = "queen"; break;
            case 1: piece = "rook"; break;
            case 2: piece = "bishop"; break;
            case 3: piece = "knight"; break;
            }

            char path[50];
            sprintf(path, "img/%s_%s.png", piece, color);
            SDL_Surface* surface = IMG_Load(path);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);

        }

        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Obrada zatvaranja prozora, ako je potrebno
                // Ovisno o strukturi vašeg programa, ovdje možete izaæi iz igre ili nešto slièno.
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Provjera da li je kliknuto na jedno od gumba za promociju
                for (int i = 0; i < 4; i++) {
                    if (x >= startX + i * FIELD_WIDHT && x < startX + (i + 1) * FIELD_WIDHT &&
                        y >= startY && y < startY + FIELD_HEIGHT) {
                        // Promocija pijuna
                        char promotionPiece;
                        switch (i) {
                        case 0: promotionPiece = (game[row][col] == 'P') ? 'Q' : 'q'; break; // Dama
                        case 1: promotionPiece = (game[row][col] == 'P') ? 'R' : 'r'; break; // Top
                        case 2: promotionPiece = (game[row][col] == 'P') ? 'B' : 'b'; break; // Lovac
                        case 3: promotionPiece = (game[row][col] == 'P') ? 'N' : 'n'; break; // Konj
                        }
                        game[row][col] = promotionPiece;
                        choosing = false;
                        break; // Izlaz iz petlje za promociju
                    }
                }
            }
        }

    }
}