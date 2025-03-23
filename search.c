#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "evaluate.c"


const int MAX_TIME = 2;

struct bestMove {
    int col;
    int score;
};


int alphaBetaSearch(bitboard rbb, bitboard ybb, int pliesRemaining, int pliesFromRoot, int alpha, int beta, int heights[7], int sideToMove, struct bestMove *bestMove) {
    int gameState = gameOver(rbb, ybb);
    if (gameState != -1) {
        if (gameState == 0) {
            return 0;
        }
        else if (gameState == sideToMove) {
            return INT16_MAX;
        }
        return INT16_MIN;
    }
    
    if (pliesRemaining == 0) 
        return evaluation(rbb, ybb, heights, sideToMove);

    int cols[7];
    memset(cols, 0, sizeof(cols));
    getPossibleCols((rbb | ybb), heights, cols);
    
    for (int i = 0; i < 7; i++) {
        if (cols[i] != 1) {
            continue;
        }
        int col = i;
        if (sideToMove == RED) 
            placePiece(&rbb, heights, col);
        else
            placePiece(&ybb, heights, col);

        int oppSideToMove = sideToMove == RED ? YELLOW : RED;
        int evaluation = -alphaBetaSearch(rbb, ybb, pliesRemaining - 1, pliesFromRoot + 1, -beta, - alpha, heights, oppSideToMove, bestMove);

        if (sideToMove == RED) 
            removePiece(&rbb, heights, col);
        else
            removePiece(&ybb, heights, col);

        if (evaluation > alpha) {
            if (pliesFromRoot == 0) {
                bestMove->col = col;
                bestMove->score = evaluation;
            }
            alpha = evaluation;
        }

        if (alpha >= beta) {
            break;
        }
    }
    return alpha;
}

struct bestMove runIterativeDeepening(bitboard rbb, bitboard ybb, int heights[7], int sideToMove, struct bestMove *prevBestMove) {
    time_t start_time, current_time;
    double elapsed_time;
    start_time = time(NULL);
    for (int i = 1; i < 43; i++) {
        alphaBetaSearch(rbb, ybb, i, 0, INT16_MIN, INT16_MAX, heights, sideToMove, prevBestMove);
        
        current_time = time(NULL);
        if (difftime(current_time, start_time) >= MAX_TIME) {
            break;
        }
        printf("%d ", i);
    }
    return *prevBestMove;
}

int search(bitboard rbb, bitboard ybb, int heights[7], int sideToMove) {
    struct bestMove bestMove;
    bestMove.col = 3;
    bestMove.score = INT16_MIN;

    bestMove = runIterativeDeepening(rbb, ybb, heights, sideToMove, &bestMove);
    return bestMove.col;
}