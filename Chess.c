#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"
#include "SDL.h"
#include "SDL_image.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define FIELD_WIDHT 80
#define FIELD_HEIGHT 80

#define MAX_PAWN_MOVES 3
#define MAX_ROOK_MOVES 14
#define MAX_KNIGHT_MOVES 8
#define MAX_BISHOP_MOVES 13
#define MAX_QUEEN_MOVES 27
#define MAX_KING_MOVES 8


typedef struct {
    int row;
    int col;
} Position;

void drawBoard(SDL_Renderer* renderer, char game[8][8]);
void updateBoard(SDL_Renderer* renderer, char game[8][8], Position oldPos, Position newPos);
int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves);
Position* getPossibleMoves(char game[8][8], int turn, Position startPos, int* moveCount, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
void drawPiece(SDL_Renderer* renderer, char piece, int row, int col);
void showPossibleMoves(SDL_Renderer* renderer, char game[8][8], Position possibleMoves[], int count);
void showCurrentlyChosen(SDL_Renderer* renderer, char game[8][8], Position chosenPosition);
void resetBoardFields(SDL_Renderer* renderer, char game[8][8], int turn, Position startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
Position* getPawnMoves(char game[8][8], Position startPos, int* count, Position* lastMove);
Position* getRookMoves(char game[8][8], Position startPos, int* count);
Position* getKnightMoves(char game[8][8], Position startPos, int* count);
Position* getBishopMoves(char game[8][8], Position startPos, int* count);
Position* getQueenMoves(char game[8][8], Position startPos, int* count);
Position* getKingMoves(char game[8][8], Position startPos, int* count, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8]);
void updateAttackedFields(char game[8][8], int attackedFields[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove);
void castling(SDL_Renderer* renderer, char game[8][8], char piece, Position clickedPos, bool hasKingMoved[2], bool hasRookMoved[4]);
void promotion(SDL_Renderer* renderer, char game[8][8], int row, int col);
void resetBoardCenter(SDL_Renderer* renderer, char game[8][8]);
bool isCheck(char game[8][8], int turn, int attackedFields[8][8]);
bool isCheckmate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
bool isStalemate(char game[8][8], int turn, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove);
void filterLegalMoves(char game[8][8], Position* moves, int* moveCount, Position startPos, int turn, bool hasKingMoved[2], bool hasRookMoved[4], Position* lastMove);
void endGame();
void drawGame();
bool isEnPassant(Position* lastMove, Position startPos, char piece, char game[8][8]);
char* isCastlingPossible(char piece, char game[8][8], bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8]);
void loadLastBoardState(char game[8][8], char* currentBoard, int* halfMoves, int* turn);
char* storeCurrentBoardState(char game[8][8], int turn, char* castlingFEN, char* enPassantFEN, int halfMoves);

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

    
    char currentBoard[90] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 0";
    int halfMoves = 0;
    int turn = 1;   // white = 1, black = -1

    bool isMoving = false;
    Position startPos = { 0, 0 };

    bool hasKingMoved[2] = { false, false };    // black, white
    bool hasRookMoved[4] = { false, false, false, false };  // black-left, black-right, white-left, white-right

    Position lastMove[2] = { {0, 0}, {0, 0} };

    bool check = false;

    int attackedFields[8][8] = { 0 };


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


    loadLastBoardState(game, currentBoard, &halfMoves, &turn);

    if (halfMoves == 0) {   // new game
        storeCurrentBoardState(game, turn, "-", "-", halfMoves);
    }

    drawBoard(renderer, game);

    while (1) {
        if (!handlePieceMovement(renderer, game, &turn, &isMoving, &startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove, check, currentBoard, &halfMoves)) {
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





void updateBoard(SDL_Renderer* renderer, char game[8][8], Position oldPos, Position newPos) {
    Position positions[2] = { oldPos, newPos };

    for (int pos = 0; pos < 2; pos++) {
        int i = positions[pos].col;
        int j = positions[pos].row;

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








int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // check
        if (isCheck(game, *turn, attackedFields)) {
            check = true;
            
            // checkmate
            if (isCheckmate(game, *turn, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                printf("%s", "CHECK-MATE!\n");
            }
        }
        else {
            check = false;

            // stalemate
            if (isStalemate(game, *turn, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                printf("%s", "STALE-MATE!\n");
            }
        }


        if (event.type == SDL_QUIT) {
            return 0; // Korisnik želi zatvoriti prozor
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int moveCount = 0;
            Position* validMoves = NULL;

            int x, y;
            SDL_GetMouseState(&x, &y);
            Position clickedPos = { y / 80, x / 80 };


            if (!(*isMoving)) {
                if (*turn == 1 && isupper(game[clickedPos.row][clickedPos.col]) || *turn == -1 && islower(game[clickedPos.row][clickedPos.col])) { // Ako je na polju figura
                    *isMoving = true;
                    *startPos = clickedPos;
                    showCurrentlyChosen(renderer, game, clickedPos);

                    validMoves = getPossibleMoves(game, *turn, *startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove);
                    
                    showPossibleMoves(renderer, game, validMoves, moveCount);
                }
            }
            else {
                // Provjera jesu li figure iste boje
                char selectedPiece = game[startPos->row][startPos->col];
                char targetPiece = game[clickedPos.row][clickedPos.col];
                if ((isupper(selectedPiece) && isupper(targetPiece)) ||
                    (islower(selectedPiece) && islower(targetPiece))) {
                    resetBoardFields(renderer, game, *turn, *startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove);

                    // Mijenjanje odabrane figure
                    *startPos = clickedPos;

                    showCurrentlyChosen(renderer, game, clickedPos);

                    validMoves = getPossibleMoves(game, *turn, *startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove);
                    showPossibleMoves(renderer, game, validMoves, moveCount);
                }
                else {
                    validMoves = getPossibleMoves(game, *turn, *startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove);

                    showPossibleMoves(renderer, game, validMoves, moveCount);

                    bool validMove = false;
                    for (int i = 0; i < moveCount; i++) {
                        if (validMoves[i].row == clickedPos.row && validMoves[i].col == clickedPos.col) {
                            validMove = true;
                            break;
                        }
                    }

                    if (validMove) {
                        resetBoardFields(renderer, game, *turn, *startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove);
                        char enPassantFEN[3] = "-\0";
                        char castlingFEN[5] = "-\0";
                        char lastMovePiece = game[lastMove[1].row][lastMove[1].col];
                        
                        char piece = game[startPos->row][startPos->col];
                        game[startPos->row][startPos->col] = ' ';
                        game[clickedPos.row][clickedPos.col] = piece;
                        *isMoving = false; // Resetiranje statusa pomicanja

                        // promotion
                        if (piece == 'p' && clickedPos.row == 7 || piece == 'P' && clickedPos.row == 0) {
                            promotion(renderer, game, clickedPos.row, clickedPos.col);
                            resetBoardCenter(renderer, game);
                        }
                        
                        // castling
                        castling(renderer, game, piece, clickedPos, hasKingMoved, hasRookMoved);

                        // el passante
                        if (isEnPassant(lastMove, *startPos, piece, game)) {
                            game[lastMove[1].row][lastMove[1].col] = ' ';
                            updateBoard(renderer, game, *startPos, lastMove[1]);    // micanje figure odnesene el passant potezom
                        }


                        *turn *= -1;    // swiching the turn
                        (*halfMoves)++;

                        // storing data
                        if (piece == 'p' || piece == 'P' && abs(startPos->row - clickedPos.row) == 2 && abs(lastMove[1].col - startPos->col) == 1) {
                            if (*turn == 1 && lastMovePiece == 'P' && lastMove[1].row == startPos->row)
                                sprintf(enPassantFEN, "%d%d", clickedPos.row - 1, clickedPos.col);
                            else if (lastMovePiece == 'p' && lastMove[1].row == startPos->row)
                                sprintf(enPassantFEN, "%d%d", clickedPos.row + 1, clickedPos.col);
                        }

                        strcpy(castlingFEN, isCastlingPossible(piece, game, hasKingMoved, hasRookMoved, attackedFields));
                        

                        strcpy(currentBoard, storeCurrentBoardState(game, *turn, castlingFEN, enPassantFEN, *halfMoves));   // spremanje stanja prije samog poteza

                                
                        // update last/previous move
                        lastMove[0] = *startPos;
                        lastMove[1] = clickedPos;


                        updateBoard(renderer, game, *startPos, clickedPos); // Poziv updateBoard funkcije
                        updateAttackedFields(game, attackedFields, *turn * -1, hasKingMoved, hasRookMoved, lastMove);  // Updating currently attacked fields
                    }
                    else {
                        // Ako potez nije valjan, resetiraj status pomicanja
                        *isMoving = false;
                        resetBoardFields(renderer, game, *turn, *startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove);
                    }
                }
            }
            free(validMoves); // Oslobađanje memorije
        }
    }
    return 1; // Korisnik ne želi zatvoriti prozor
}





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





void drawPiece(SDL_Renderer* renderer, char piece, int row, int col) {
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

    SDL_Rect destRect = { col * 80 + 20, row * 80 + 10, 40, 60 };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}


void showPossibleMoves(SDL_Renderer* renderer, char game[8][8], Position possibleMoves[], int count) {
    for (int i = 0; i < count; i++) {
        int row = possibleMoves[i].row;
        int col = possibleMoves[i].col;

        SDL_Rect rect = { col * 80, row * 80, 80, 80 };
        SDL_SetRenderDrawColor(renderer, 204, 102, 102, 255); // Crvenkasta boja za moguće poteze
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
    int row = chosenPosition.row;
    int col = chosenPosition.col;

    SDL_Rect rect = { col * 80, row * 80, 80, 80 };
    SDL_SetRenderDrawColor(renderer, 255, 51, 51, 255); // Intenzivnija crvenkasta boja za trenutno odabrano
    SDL_RenderFillRect(renderer, &rect);

    // Iscrtaj figuricu ponovo
    char piece = game[row][col];
    drawPiece(renderer, piece, row, col);
    SDL_RenderPresent(renderer);
}


void resetBoardFields(SDL_Renderer* renderer, char game[8][8], int turn, Position startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove) {
    int moveCount;
    Position* positions = getPossibleMoves(game, turn, startPos, &moveCount, hasKingMoved, hasRookMoved, attackedFields, lastMove);

    // Resetiranje izgleda polja na kojemu je figura
    if ((startPos.row + startPos.col) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Dark Brown
    }
    else {
        SDL_SetRenderDrawColor(renderer, 233, 194, 166, 255); // Light Brown (Yellowish)
    }

    SDL_Rect rect = { startPos.col * 80, startPos.row * 80, 80, 80 };
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

        SDL_Rect rect = { col * 80, row * 80, 80, 80 };
        SDL_RenderFillRect(renderer, &rect);

        // Iscrtaj figuricu ponovo ako postoji na tom polju
        char piece = game[row][col];
        if (piece != ' ') {
            drawPiece(renderer, piece, row, col);
        }
    }
    SDL_RenderPresent(renderer);

    // Oslobađanje memorije
    free(positions);
}










Position* getPawnMoves(char game[8][8], Position startPos, int* count, Position* lastMove) {
    Position* moves = (Position*)malloc(MAX_PAWN_MOVES * sizeof(Position));
    *count = 0;

    char pawn = game[startPos.row][startPos.col];
    int direction = islower(pawn) ? 1 : -1; // Za crne pješake ide dolje, za bijele ide gore

    // Provjera je li pijun na rubu ploče
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

        // Posebna pravila za početni pomak od dva polja za pješake
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


Position* getRookMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_ROOK_MOVES * sizeof(Position));
    *count = 0;

    char rook = game[startPos.row][startPos.col];

    // Provjera vodoravno prema desno
    for (int j = startPos.col + 1; j < 8; j++) {
        if (game[startPos.row][j] == ' ') {
            moves[*count].row = startPos.row;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startPos.row][j])) || (islower(rook) && isupper(game[startPos.row][j]))) {
                moves[*count].row = startPos.row;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera vodoravno prema lijevo
    for (int j = startPos.col - 1; j >= 0; j--) {
        if (game[startPos.row][j] == ' ') {
            moves[*count].row = startPos.row;
            moves[*count].col = j;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[startPos.row][j])) || (islower(rook) && isupper(game[startPos.row][j]))) {
                moves[*count].row = startPos.row;
                moves[*count].col = j;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema gore
    for (int i = startPos.row - 1; i >= 0; i--) {
        if (game[i][startPos.col] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startPos.col;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startPos.col])) || (islower(rook) && isupper(game[i][startPos.col]))) {
                moves[*count].row = i;
                moves[*count].col = startPos.col;
                (*count)++;
            }
            break;
        }
    }

    // Provjera okomito prema dolje
    for (int i = startPos.row + 1; i < 8; i++) {
        if (game[i][startPos.col] == ' ') {
            moves[*count].row = i;
            moves[*count].col = startPos.col;
            (*count)++;
        }
        else {
            if ((isupper(rook) && islower(game[i][startPos.col])) || (islower(rook) && isupper(game[i][startPos.col]))) {
                moves[*count].row = i;
                moves[*count].col = startPos.col;
                (*count)++;
            }
            break;
        }
    }

    return moves;
}


Position* getKnightMoves(char game[8][8], Position startPos, int* count) {
    Position* moves = (Position*)malloc(MAX_KNIGHT_MOVES * sizeof(Position));
    *count = 0;

    char knight = game[startPos.row][startPos.col];

    // Definiramo sve moguće poteze konja
    int rowMoves[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int colMoves[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

    for (int i = 0; i < 8; i++) {
        int newRow = startPos.row + rowMoves[i];
        int newCol = startPos.col + colMoves[i];

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
            // Ako je unutar ploče

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
            // Kralj ne može preći preko drugih figura, pa nema potrebe za daljnjom provjerom
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

                free(moves); // Oslobađanje memorije
            }
        }
    }
}


void castling(SDL_Renderer* renderer, char game[8][8], char piece, Position clickedPos, bool hasKingMoved[2], bool hasRookMoved[4]) {
    // castling
    if (piece == 'k') { // black
        if (hasKingMoved[0] == false && clickedPos.row == 0) {
            if (clickedPos.col == 2 && hasRookMoved[0] == false) {
                game[0][0] = ' ';
                game[0][3] = 'r';
                hasRookMoved[0] = true;
                Position rookPosOld = { 0, 0 };
                Position rookPosNew = { 0, 3 };
                updateBoard(renderer, game, rookPosOld, rookPosNew);
            }
            else if (clickedPos.col == 6 && hasRookMoved[1] == false) {
                game[0][7] = ' ';
                game[0][5] = 'r';
                hasRookMoved[1] = true;
                Position rookPosOld = { 0, 7 };
                Position rookPosNew = { 0, 5 };
                updateBoard(renderer, game, rookPosOld, rookPosNew);
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
                updateBoard(renderer, game, rookPosOld, rookPosNew);
            }
            else if (clickedPos.col == 6 && hasRookMoved[3] == false) {
                game[7][7] = ' ';
                game[7][5] = 'R';
                hasRookMoved[3] = true;
                Position rookPosOld = { 7, 7 };
                Position rookPosNew = { 7, 5 };
                updateBoard(renderer, game, rookPosOld, rookPosNew);
            }
        }
        hasKingMoved[1] = true;
    }
}



void promotion(SDL_Renderer* renderer, char game[8][8], int row, int col) {
    // Koordinate i veličina gumba za promociju
    int buttonWidth = 40;
    int buttonHeight = 60;
    int startX = (SCREEN_WIDTH - FIELD_WIDHT * 4) / 2;
    int startY = (SCREEN_HEIGHT - FIELD_HEIGHT) / 2;

    // Okvir oko prozora
    SDL_Rect borderRect = { startX - 10, startY - 10, FIELD_WIDHT * 4 + 20, FIELD_HEIGHT + 20};
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
                // Ovisno o strukturi vašeg programa, ovdje možete izaći iz igre ili nešto slično.
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

    // Osvježite prikaz ploče ovdje, ako je potrebno
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

void endGame() {
    // TODO
}

void drawGame() {
    // TODO
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



char* isCastlingPossible(char piece, char game[8][8], bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8]) {
    static bool possibleCastling[4]; // static kako bi se sačuvala vrijednost između poziva

    // Resetiraj niz na false
    for (int i = 0; i < 4; i++) {
        possibleCastling[i] = false;
    }

    
    for (int i = 0; i < 2; i++) {
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





void loadLastBoardState(char game[8][8], char* currentBoard, int* halfMoves, int* turn) {
    FILE* historyFile = fopen("game_history.txt", "r");
    if (historyFile != NULL) {
        char lastLine[100];
        while (fgets(lastLine, sizeof(lastLine), historyFile) != NULL) {
            // samo čitamo do kraja datoteke
        }
        fclose(historyFile);

        strcpy(currentBoard, lastLine);

        // Razdvajamo na dijelove
        char* token = strtok(lastLine, " ");
        int row = 0, col = 0;

        // Postavljanje ploče
        while (token[0] != 'w' && token[0] != 'b') {
            for (int i = 0; i < strlen(token); i++) {
                if (isdigit(token[i])) {
                    int spaces = token[i] - '0';
                    while (spaces--) {
                        game[row][col++] = ' ';
                    }
                }
                else {
                    game[row][col++] = token[i];
                }
            }
            row++;
            col = 0;
            token = strtok(NULL, " ");
        }

        // Postavljanje tura
        *turn = (token[0] == 'w') ? 1 : -1;

        // Preskacemo prava rokade
        token = strtok(NULL, " ");

        // Preskacemo en passant
        token = strtok(NULL, " ");

        // Postavljanje halfMoves
        token = strtok(NULL, " ");
        *halfMoves = atoi(token);
    }
}



char* storeCurrentBoardState(char game[8][8], int turn, char* castlingFEN, char* enPassantFEN, int halfMoves) {
    FILE* historyFile = fopen("game_history.txt", "a");
    if (historyFile == NULL) {
        perror("Could not open the file");
        return NULL;
    }

    char currentBoard[90];
    int idx = 0;

    for (int row = 0; row < 8; ++row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            char c = game[row][col];
            if (c == ' ') {
                emptyCount++;
            }
            else {
                if (emptyCount != 0) {
                    currentBoard[idx++] = '0' + emptyCount;
                }
                emptyCount = 0;
                currentBoard[idx++] = c;
            }
        }
        if (emptyCount != 0) {
            currentBoard[idx++] = '0' + emptyCount;
        }
        currentBoard[idx++] = ' ';
    }

    currentBoard[idx++] = (turn == 1) ? 'w' : 'b';
    currentBoard[idx++] = ' ';

    strcpy(currentBoard + idx, castlingFEN);
    idx += strlen(castlingFEN);

    currentBoard[idx++] = ' ';

    strcpy(currentBoard + idx, enPassantFEN);
    idx += strlen(enPassantFEN);

    currentBoard[idx++] = ' ';

    char halfMoveString[10];
    snprintf(halfMoveString, 10, "%d", halfMoves);
    strcpy(currentBoard + idx, halfMoveString);

    currentBoard[idx++] = ' ';

    char fullMoveString[10];
    snprintf(fullMoveString, 10, "%d", halfMoves / 2);
    strcpy(currentBoard + idx, fullMoveString);

    fprintf(historyFile, "%s\n", currentBoard);
    fclose(historyFile);

    return currentBoard;
}



bool checkThreefoldRepetition(const char* lastBoardState) {
    FILE* historyFile = fopen("game_history.txt", "r");
    if (historyFile == NULL) {
        perror("Could not open the file");
        return false;
    }

    char line[100];
    int counter = 0;

    while (fgets(line, sizeof(line), historyFile) != NULL) {
        // Uzimamo samo dio koji opisuje ploču, ignoriramo halfMoves i turn
        char* delimiterPos = strchr(line, ' ');
        if (delimiterPos) {
            *delimiterPos = '\0'; // Postavljamo kraj stringa
        }

        char* lastDelimiterPos = strchr(lastBoardState, ' ');
        if (lastDelimiterPos) {
            *lastDelimiterPos = '\0'; // Postavljamo kraj stringa
        }

        if (strcmp(line, lastBoardState) == 0) {
            counter++;
        }

        // Vraćamo originalne vrijednosti
        if (delimiterPos) {
            *delimiterPos = ' ';
        }

        if (lastDelimiterPos) {
            *lastDelimiterPos = ' ';
        }
    }

    fclose(historyFile);

    return (counter >= 3);
}
