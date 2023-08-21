#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "SDL.h"
#include "SDL_image.h"

#define MAX_MOVES 8

typedef struct {
    int row;
    int col;
} Position;

void drawBoard(SDL_Renderer* renderer, char game[8][8]);
void updateBoard(SDL_Renderer* renderer, char game[8][8], int oldX, int oldY, int newX, int newY);
int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* isMoving, int* startRow, int* startCol);
Position* getPawnMoves(char game[8][8], int startRow, int startCol, int* count);


int main(int argc, char* argv[])
{
    char game[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    char fen_string[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    int isMoving = 0;
    int startRow, startCol;


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chess Board",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 640,
        SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    drawBoard(renderer, game);


    while (1) {
        if (!handlePieceMovement(renderer, game, &isMoving, &startRow, &startCol)) {
            break; // Ako korisnik želi zatvoriti prozor
        }
        SDL_Delay(100);
    }


    SDL_Delay(5000); // Čekanje 5000 milisekundi (5 sekundi) prije zatvaranja

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}


void drawBoard(SDL_Renderer* renderer, char game[8][8]) {
    // Crtanje ploče
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            SDL_Rect rect = { i * 80, j * 80, 80, 80 };
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Dark Brown
            }
            else {
                SDL_SetRenderDrawColor(renderer, 233, 194, 166, 255); // Light Brown (Yellowish)
            }
            SDL_RenderFillRect(renderer, &rect);

            // Provjera da li je figura na trenutnom polju i njen prikaz
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

                SDL_Rect destRect = { i * 80 + 20, j * 80 + 10, 40, 60 }; // Pozicija i dimenzija gdje će se figura renderirati
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
                SDL_DestroyTexture(texture);
            }
        }
    }
    SDL_RenderPresent(renderer);
}


void updateBoard(SDL_Renderer* renderer, char game[8][8], int oldX, int oldY, int newX, int newY) {
    int positions[2][2] = { {oldX, oldY}, {newX, newY} };

    for (int pos = 0; pos < 2; pos++) {
        int i = positions[pos][0];
        int j = positions[pos][1];

        SDL_Rect rect = { i * 80, j * 80, 80, 80 };
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

            SDL_Rect destRect = { i * 80 + 20, j * 80 + 10, 40, 60 };
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
    }
    SDL_RenderPresent(renderer);
}



int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* isMoving, int* startRow, int* startCol) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 0; // Znači da korisnik želi zatvoriti prozor
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            int row = y / 80;
            int col = x / 80;
            if (!(*isMoving)) {
                if (game[row][col] != ' ') { // Ako je na polju figura
                    *isMoving = 1;
                    *startRow = row;
                    *startCol = col;
                }
            }
            else {
                // Provjerite jesu li figure iste boje
                char selectedPiece = game[*startRow][*startCol];
                char targetPiece = game[row][col];
                if ((isupper(selectedPiece) && isupper(targetPiece)) ||
                    (islower(selectedPiece) && islower(targetPiece))) {
                    // Mijenjanje odabrane figure
                    *startRow = row;
                    *startCol = col;
                }
                else {
                    char piece = game[*startRow][*startCol];
                    game[*startRow][*startCol] = ' ';
                    game[row][col] = piece;
                    *isMoving = 0; // Resetiranje statusa pomicanja
                    updateBoard(renderer, game, *startCol, *startRow, col, row); // Poziv updateBoard
                }
            }
        }
    }
    return 1; // Znači da korisnik ne želi zatvoriti prozor
}


Position* getPawnMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char pawn = game[startRow][startCol];
    int direction = islower(pawn) ? 1 : -1; // Za crne pješake ide dolje, za bijele ide gore

    // Provjerite može li se pomaknuti naprijed
    if (game[startRow + direction][startCol] == ' ') {
        moves[*count].row = startRow + direction;
        moves[*count].col = startCol;
        (*count)++;
    }

    // Provjerite može li se pojesti neka figura dijagonalno
    if (startCol - 1 >= 0 && ((isupper(pawn) && islower(game[startRow + direction][startCol - 1])) ||
        (islower(pawn) && isupper(game[startRow + direction][startCol - 1])))) {
        moves[*count].row = startRow + direction;
        moves[*count].col = startCol - 1;
        (*count)++;
    }

    if (startCol + 1 < 8 && ((isupper(pawn) && islower(game[startRow + direction][startCol + 1])) ||
        (islower(pawn) && isupper(game[startRow + direction][startCol + 1])))) {
        moves[*count].row = startRow + direction;
        moves[*count].col = startCol + 1;
        (*count)++;
    }

    // Posebna pravila za početni pomak od dva polja za pješake
    if ((isupper(pawn) && startRow == 6) || (islower(pawn) && startRow == 1)) {
        if (game[startRow + direction][startCol] == ' ' && game[startRow + 2 * direction][startCol] == ' ') {
            moves[*count].row = startRow + 2 * direction;
            moves[*count].col = startCol;
            (*count)++;
        }
    }

    return moves;
}
