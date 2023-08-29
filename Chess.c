#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"

#include "SDL.h"
#include "SDL_image.h"

#include "Position.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "Board.h"
#include "Movement.h"
#include "Game.h"



#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 680
#define BOARD_WIDTH 640
#define BOARD_HEIGHT 640
#define FIELD_WIDHT 80
#define FIELD_HEIGHT 80


int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves, int* stallingMoves);
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

    int stallingMoves = 0;

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
        SCREEN_WIDTH, SCREEN_HEIGHT,
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
        if (!handlePieceMovement(renderer, game, &turn, &isMoving, &startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove, check, currentBoard, &halfMoves, &stallingMoves)) {
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


int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves, int* stallingMoves) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // check
        if (isCheck(game, *turn, attackedFields)) {
            check = true;
            
            // checkmate
            if (isCheckmate(game, *turn, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                printf("%s", "CHECK-MATE!\n");
                endGame(renderer);
            }
        }
        else {
            check = false;

            // stalemate
            if (isStalemate(game, *turn, hasKingMoved, hasRookMoved, attackedFields, lastMove)) {
                printf("%s", "STALE-MATE!\n");
                drawGame(renderer);
            }
            else if (isThreefoldRepetition(currentBoard)) {
                printf("%s", "THREEFOLD REPETITION!\n");
                drawGame(renderer);
            }
            else if (is50MoveRule(*stallingMoves)) {
                printf("%s", "50-MOVE RULE!\n");
                drawGame(renderer);
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
                        char enPassantFEN[3] = "-";
                        char castlingFEN[5] = "-";
                        char lastMovePiece = game[lastMove[1].row][lastMove[1].col];

                        // update moves without pawn move or piece taken (50-Move rule)
                        if (targetPiece == ' ' && tolower(selectedPiece) != 'p') {
                            (*stallingMoves)++;
                        }
                        else {
                            *stallingMoves = 0;
                        }
                        
                        game[startPos->row][startPos->col] = ' ';
                        game[clickedPos.row][clickedPos.col] = selectedPiece;
                        *isMoving = false; // Resetiranje statusa pomicanja

                        // promotion
                        if (selectedPiece == 'p' && clickedPos.row == 7 || selectedPiece == 'P' && clickedPos.row == 0) {
                            promotion(renderer, game, clickedPos.row, clickedPos.col);
                            resetBoardCenter(renderer, game);
                        }
                        
                        // castling
                        Position* rookPositions = castling(game, selectedPiece, clickedPos, hasKingMoved, hasRookMoved);
                        if (rookPositions != NULL) {
                            updateBoard(renderer, game, rookPositions[0], rookPositions[1]);
                        }

                        // el passante
                        if (isEnPassant(lastMove, *startPos, selectedPiece, game)) {
                            game[lastMove[1].row][lastMove[1].col] = ' ';
                            updateBoard(renderer, game, *startPos, lastMove[1]);    // micanje figure odnesene el passant potezom
                        }


                        // update last/previous move
                        lastMove[0] = *startPos;
                        lastMove[1] = clickedPos;

                        *turn *= -1;    // swiching the turn
                        (*halfMoves)++;


                        // storing data
                        if (selectedPiece == 'p' || selectedPiece == 'P' && abs(startPos->row - clickedPos.row) == 2) {
                            if (*turn == -1 && (clickedPos.col - 1 >= 0 && game[clickedPos.row][clickedPos.col - 1] == 'p' || 
                                clickedPos.col + 1 < 8 && game[clickedPos.row][clickedPos.col + 1] == 'p'))
                                sprintf(enPassantFEN, "%d%d", clickedPos.row + 1, clickedPos.col);
                            else if (*turn == 1 && (clickedPos.col - 1 >= 0 && game[clickedPos.row][clickedPos.col - 1] == 'P' ||
                                clickedPos.col + 1 < 8 && game[clickedPos.row][clickedPos.col + 1] == 'P'))
                                sprintf(enPassantFEN, "%d%d", clickedPos.row - 1, clickedPos.col);
                        }

                        Position currentMove[2] = { *startPos, clickedPos };
                        strcpy(castlingFEN, isCastlingPossible(selectedPiece, game, hasKingMoved, hasRookMoved, currentMove));

                        strcpy(currentBoard, storeCurrentBoardState(game, *turn, castlingFEN, enPassantFEN, *halfMoves));   // spremanje stanja prije samog poteza



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


void loadLastBoardState(char game[8][8], char* currentBoard, int* halfMoves, int* turn) {
    FILE* historyFile = fopen("data/game_history.txt", "r");
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
        char* boardToken = strtok(token, "/");
        while (boardToken) {
            for (int i = 0; i < strlen(boardToken); i++) {
                if (isdigit(boardToken[i])) {
                    int spaces = boardToken[i] - '0';
                    while (spaces--) {
                        game[row][col++] = ' ';
                    }
                }
                else {
                    game[row][col++] = boardToken[i];
                }
            }
            row++;
            col = 0;
            boardToken = strtok(NULL, "/ ");
        }

        strcpy(lastLine, currentBoard);
        token = strtok(lastLine, " ");
        token = strtok(NULL, " ");
        
        // Postavljanje turna
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
    FILE* historyFile = fopen("data/game_history.txt", "a");
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
        if (row != 7)
            currentBoard[idx++] = '/';
        else
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

    char halfMoveString[5];
    snprintf(halfMoveString, 5, "%d", halfMoves);
    strcpy(currentBoard + idx, halfMoveString);
    idx += strlen(halfMoveString);

    currentBoard[idx++] = ' ';

    char fullMoveString[5];
    snprintf(fullMoveString, 5, "%d", halfMoves / 2);
    strcpy(currentBoard + idx, fullMoveString);

    fprintf(historyFile, "%s\n", currentBoard);
    fclose(historyFile);

    return currentBoard;
}