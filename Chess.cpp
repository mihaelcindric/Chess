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
Position* getRookMoves(char game[8][8], int startRow, int startCol, int* count);
Position* getKnightMoves(char game[8][8], int startRow, int startCol, int* count);
Position* getBishopMoves(char game[8][8], int startRow, int startCol, int* count);
Position* getQueenMoves(char game[8][8], int startRow, int startCol, int* count);
Position* getKingMoves(char game[8][8], int startRow, int startCol, int* count);



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
            return 0; // Korisnik želi zatvoriti prozor
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
                // Provjera jesu li figure iste boje
                char selectedPiece = game[*startRow][*startCol];
                char targetPiece = game[row][col];
                if ((isupper(selectedPiece) && isupper(targetPiece)) ||
                    (islower(selectedPiece) && islower(targetPiece))) {
                    // Mijenjanje odabrane figure
                    *startRow = row;
                    *startCol = col;
                }
                else {
                    int moveCount;
                    Position* validMoves = NULL;
                    switch (tolower(selectedPiece)) {
                    case 'p':
                        validMoves = getPawnMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    case 'r':
                        validMoves = getRookMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    case 'n':
                        validMoves = getKnightMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    case 'b':
                        validMoves = getBishopMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    case 'q':
                        validMoves = getQueenMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    case 'k':
                        validMoves = getKingMoves(game, *startRow, *startCol, &moveCount);
                        break;
                    }

                    int validMove = 0;
                    for (int i = 0; i < moveCount; i++) {
                        if (validMoves[i].row == row && validMoves[i].col == col) {
                            validMove = 1;
                            break;
                        }
                    }
                    free(validMoves); // Oslobađanje memorije

                    if (validMove) {
                        char piece = game[*startRow][*startCol];
                        game[*startRow][*startCol] = ' ';
                        game[row][col] = piece;
                        *isMoving = 0; // Resetiranje statusa pomicanja
                        updateBoard(renderer, game, *startCol, *startRow, col, row); // Poziv updateBoard funkcije
                    }
                    else {
                        // Ako potez nije valjan, resetiraj status pomicanja
                        *isMoving = 0;
                    }
                }
            }
        }
    }
    return 1; // Korisnik ne želi zatvoriti prozor
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


Position* getRookMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char rook = game[startRow][startCol];

    // Provjera vodoravno prema desno
    for (int j = startCol + 1; j < 8; j++) {
        if (game[startRow][j] == ' ') {
            moves[*count].row = startRow;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startRow][j])) || (islower(rook) && isupper(game[startRow][j]))) {
                moves[*count].row = startRow;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera vodoravno prema lijevo
    for (int j = startCol - 1; j >= 0; j--) {
        if (game[startRow][j] == ' ') {
            moves[*count].row = startRow;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startRow][j])) || (islower(rook) && isupper(game[startRow][j]))) {
                moves[*count].row = startRow;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema gore
    for (int i = startRow - 1; i >= 0; i--) {
        if (game[i][startCol] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startCol;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startCol])) || (islower(rook) && isupper(game[i][startCol]))) {
                moves[*count].row = i;
                moves[*count].col = startCol;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema dolje
    for (int i = startRow + 1; i < 8; i++) {
        if (game[i][startCol] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startCol;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startCol])) || (islower(rook) && isupper(game[i][startCol]))) {
                moves[*count].row = i;
                moves[*count].col = startCol;
                (*count)++;
            }
            break;
        }
    }

    return moves;
}


Position* getKnightMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char knight = game[startRow][startCol];

    // Definiramo sve moguće poteze konja
    int rowMoves[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int colMoves[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

    for (int i = 0; i < 8; i++) {
        int newRow = startRow + rowMoves[i];
        int newCol = startCol + colMoves[i];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) { // Provjera je li unutar ploče
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


Position* getBishopMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char bishop = game[startRow][startCol];

    // Definiramo smjerove kretanja lovca: gore-lijevo, gore-desno, dolje-lijevo, dolje-desno
    int rowMoves[4] = { -1, -1, 1, 1 };
    int colMoves[4] = { -1, 1, -1, 1 };

    for (int dir = 0; dir < 4; dir++) {
        int step = 1;

        while (true) {
            int newRow = startRow + step * rowMoves[dir];
            int newCol = startCol + step * colMoves[dir];

            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                // Ako je unutar ploče

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

                step++; // Pomičemo se dalje u trenutnom smjeru
            }
            else {
                break; // Izvan ploče
            }
        }
    }

    return moves;
}


Position* getQueenMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char queen = game[startRow][startCol];

    // Definiramo smjerove kretanja kraljice: horizontalno, vertikalno i dijagonalno
    int rowMoves[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    int colMoves[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };

    for (int dir = 0; dir < 8; dir++) {
        int step = 1;

        while (true) {
            int newRow = startRow + step * rowMoves[dir];
            int newCol = startCol + step * colMoves[dir];

            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                // Ako je unutar ploče

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

                step++; // Pomičemo se dalje u trenutnom smjeru
            }
            else {
                break; // Izvan ploče
            }
        }
    }

    return moves;
}


Position* getKingMoves(char game[8][8], int startRow, int startCol, int* count) {
    Position* moves = (Position*)malloc(MAX_MOVES * sizeof(Position));
    *count = 0;

    char king = game[startRow][startCol];

    // Definiramo smjerove kretanja kralja: horizontalno, vertikalno i dijagonalno
    int rowMoves[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    int colMoves[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };

    for (int dir = 0; dir < 8; dir++) {
        int newRow = startRow + rowMoves[dir];
        int newCol = startCol + colMoves[dir];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            // Ako je unutar ploče

            if (game[newRow][newCol] == ' ') {
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
            // Kralj ne može preći preko drugih figura, pa nema potrebe za daljnjom provjerom
        }
    }

    return moves;
}
