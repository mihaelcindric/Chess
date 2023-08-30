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


int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves, int* stallingMoves, char captured[2][15]);
void loadLastBoardState(char game[8][8], char* currentBoard, int* halfMoves, int* turn); char* storeCurrentBoardState(char game[8][8], int turn, char* castlingFEN, char* enPassantFEN, int halfMoves);
char* storeCurrentBoardState(char game[8][8], int turn, char* castlingFEN, char* enPassantFEN, int halfMoves);
void showGameOptions(SDL_Renderer* renderer, char game[8][8], char* currentBoard, int* halfMoves, int* turn);
void showCapturedPiecesAndResetButton(SDL_Renderer* renderer, char captured[2][15]);
void updateCapturedPieces(char piece, int turn, char captured[2][15]);

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
    char captured[2][15];
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 15; j++) {
            captured[i][j] = ' ';
        }
    }


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chess Board",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH + 130, SCREEN_HEIGHT,
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
    showCapturedPiecesAndResetButton(renderer, captured);
    showGameOptions(renderer, game, currentBoard, &halfMoves, &turn);
    drawBoard(renderer, game);

    while (1) {
        if (!handlePieceMovement(renderer, game, &turn, &isMoving, &startPos, hasKingMoved, hasRookMoved, attackedFields, lastMove, check, currentBoard, &halfMoves, &stallingMoves, captured)) {
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


int handlePieceMovement(SDL_Renderer* renderer, char game[8][8], int* turn, bool* isMoving, Position* startPos, bool hasKingMoved[2], bool hasRookMoved[4], int attackedFields[8][8], Position* lastMove, bool check, char* currentBoard, int* halfMoves, int* stallingMoves, char captured[2][15]) {
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
            Position clickedPos = { (y - 20) / 80, (x - 20) / 80 };

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
                            updateCapturedPieces(lastMovePiece, *turn, captured);
                            showCapturedPiecesAndResetButton(renderer, captured);
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

                        strcpy(currentBoard, storeCurrentBoardState(game, *turn, castlingFEN, enPassantFEN, *halfMoves));   // spremanje stanja nakon poteza



                        updateBoard(renderer, game, *startPos, clickedPos); // Poziv updateBoard funkcije
                        updateAttackedFields(game, attackedFields, *turn * -1, hasKingMoved, hasRookMoved, lastMove);  // Updating currently attacked fields
                        
                        if (targetPiece != ' ') {
                            updateCapturedPieces(targetPiece, *turn * -1, captured);
                            showCapturedPiecesAndResetButton(renderer, captured);
                        }

                        for (int i = 0; i < 2; i++) {
                            for (int j = 0; j < 15; j++) {
                                printf("%c-", captured[i][j]);
                            }
                            printf("\n");
                        }
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


void showGameOptions(SDL_Renderer* renderer, char game[8][8], char* currentBoard, int* halfMoves, int* turn) {
    FILE* file = fopen("data/game_history.txt", "r");

    if (file == NULL) {
        remove("data/game_history.txt");
        storeCurrentBoardState(game, *turn, "-", "-", *halfMoves);
        return;
    }
    fclose(file);

    int buttonWidth = 150;
    int buttonHeight = 80;
    int gap = 30; // Dodali smo varijablu za razmak između gumba
    int totalWidth = (buttonWidth * 2) + gap;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int startY = (SCREEN_HEIGHT - buttonHeight) / 2;

    SDL_Rect mainBorder = { startX - 20, startY - 20, totalWidth + 40, buttonHeight + 40 };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &mainBorder);

    SDL_Rect mainBackground = { startX - 10, startY - 10, totalWidth + 20, buttonHeight + 20 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &mainBackground);

    char buttonImages[2][30];
    sprintf(buttonImages[0], "img/%s.png", "circle_large");
    sprintf(buttonImages[1], "img/%s.png", "circle_small");

    SDL_Rect buttons[2];
    SDL_Rect buttonBorders[2]; // Dodali smo ovo za crne okvire

    for (int i = 0; i < 2; i++) {
        buttons[i].x = startX + i * (buttonWidth + gap);
        buttons[i].y = startY;
        buttons[i].w = buttonWidth;
        buttons[i].h = buttonHeight;

        // Crni okviri
        buttonBorders[i] = buttons[i];
        buttonBorders[i].x -= 5;
        buttonBorders[i].y -= 5;
        buttonBorders[i].w += 10;
        buttonBorders[i].h += 10;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &buttonBorders[i]); // Crta crne okvire

        SDL_Surface* surface = IMG_Load(buttonImages[i]);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        SDL_RenderCopy(renderer, texture, NULL, &buttons[i]);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                return;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                for (int i = 0; i < 2; i++) {
                    if (x >= buttons[i].x && x <= buttons[i].x + buttonWidth &&
                        y >= buttons[i].y && y <= buttons[i].y + buttonHeight) {

                        if (i == 0) {
                            loadLastBoardState(game, currentBoard, halfMoves, turn);
                            return;
                        }
                        else if (i == 1) {
                            remove("data/game_history.txt");
                            storeCurrentBoardState(game, *turn, "-", "-", *halfMoves);
                            return;
                        }
                    }
                }
            }
        }
    }
}


void updateCapturedPieces(char piece, int turn, char captured[2][15]) {
    int color;

    if (turn == 1) {    // white made the capture
        color = 0;
    }
    else {  // black made the capture
        color = 1;
    }
    
    for (int i = 0; i < 15; i++) {
        if (captured[color][i] == ' ') {
            captured[color][i] = piece;
            break;
        }
    }
}


void showCapturedPiecesAndResetButton(SDL_Renderer* renderer, char captured[2][15]) {
    // Sivi prostor
    SDL_Rect greyRect = { SCREEN_WIDTH, 0, 130, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB za sivu
    SDL_RenderFillRect(renderer, &greyRect);

    // Iscrtavanje bijelih figura
    int offsetTop = 5;
    int offsetLeft = 5;
    for (int i = 0; i < 15; i++) {
        if (captured[1][i] == ' ')
            break;

        char color[6];
        strcpy(color, islower(captured[1][i]) ? "black" : "white");
        char pieceName[7];
        sprintf(pieceName, "%s", (char* []) { "rook", "knight", "bishop", "queen", "king", "pawn" }[strchr("rnbqkp", tolower(captured[1][i])) - "rnbqkp"]);
        char path[50];
        sprintf(path, "img/%s_%s.png", pieceName, color);
        SDL_Surface* surface = IMG_Load(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect destRect = { SCREEN_WIDTH + offsetLeft, offsetTop, 20, 30 };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_DestroyTexture(texture);

        offsetLeft += 25;  // Dodajte horizontalni pomak za sljedeću figuru
        if (i % 5 == 4) {
            offsetTop += 40;  // Dodajte vertikalni pomak za sljedeći red
            offsetLeft = 5;  // Resetirajte horizontalni pomak
        }
    }

    // Iscrtavanje crnih figura
    offsetTop = SCREEN_HEIGHT - 5 - 120;
    offsetLeft = 5;
    for (int i = 0; i < 15; i++) {
        if (captured[0][i] == ' ')
            break;

        char color[6];
        strcpy(color, islower(captured[0][i]) ? "black" : "white");
        char pieceName[7];
        sprintf(pieceName, "%s", (char* []) { "rook", "knight", "bishop", "queen", "king", "pawn" }[strchr("rnbqkp", tolower(captured[0][i])) - "rnbqkp"]);
        char path[50];
        sprintf(path, "img/%s_%s.png", pieceName, color);
        SDL_Surface* surface = IMG_Load(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect destRect = { SCREEN_WIDTH + offsetLeft, offsetTop, 20, 30 };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
        SDL_DestroyTexture(texture);
        offsetLeft += 25;  // Dodajte horizontalni pomak za sljedeću figuru
        if (i % 5 == 4) {
            offsetTop += 40;  // Dodajte vertikalni pomak za sljedeći red
            offsetLeft = 5;  // Resetirajte horizontalni pomak
        }
    }

    // Reset game gumb
    SDL_Rect buttonRect = { SCREEN_WIDTH + 25, (SCREEN_HEIGHT - 80) / 2, 80, 80 };

    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255); // Crna boja
    SDL_RenderFillRect(renderer, &buttonRect);

    // Učitaj i postavi sliku za gumb
    SDL_Surface* circleSurface = IMG_Load("img/circle_small.png");
    SDL_Texture* circleTexture = SDL_CreateTextureFromSurface(renderer, circleSurface);
    SDL_FreeSurface(circleSurface);

    SDL_RenderCopy(renderer, circleTexture, NULL, &buttonRect);
    SDL_DestroyTexture(circleTexture);

    SDL_RenderPresent(renderer);
}

