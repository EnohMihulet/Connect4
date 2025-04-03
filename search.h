#pragma once 
#include <stdint.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <limits.h> 
#include "transposition.h" 
#include "zobrist.h"

// Define constants as static constants. 
static const int MAX_TIME = 60;
static const int INFINITY = 999999;
static const int LOST = -99999;
static const int WON = 99999;
static const int POSSIBLE_FOUR_IN_ROW = 10;
static const int THREE_IN_A_ROW = 100;
static const int TWO_IN_A_ROW = 10;

// Define a typedef for the best move structure.
typedef struct {
    int col;
    int score;
} BestMove;

// Define the SearchContext structure. 
typedef struct {
    time_t start_time; 
    bool searchCanceled; 
    BestMove bestMoveThisIteration; 
    BestMove bestMove; 
} SearchContext;


int evaluation(bitboard rbb, bitboard ybb, int heights[7], int sideToMove) {
    int sign = sideToMove == RED ? 1 : -1;
    return sign * POSSIBLE_FOUR_IN_ROW * (fourInARowCount(ybb) - fourInARowCount(rbb));
}

int alphaBetaSearch(bitboard rbb, bitboard ybb, int pliesRemaining, int pliesFromRoot, int alpha, int beta, int heights[7], 
    int sideToMove, uint64_t hashVal, uint64_t randVals[86], SearchContext *ctx, TTEntry tTable[TT_SIZE]) { 
    if (ctx->searchCanceled) { return 0; }

    int gameState = gameOver(rbb, ybb);
    if (gameState != -1) {
        if (gameState == 0) { return 0; }
        return LOST + pliesFromRoot;
    }

    int ttEval = getEvaluation(hashVal, tTable);
    if (ttEval != -1) {
        TTEntry entry = tTable[getIndex(hashVal)];
        if (entry.depth >= pliesRemaining) {
            if (entry.nodeType == NODE_TYPE_UPPERBOUND) {
                beta = entry.score < beta ? entry.score : beta;
            } else if (entry.nodeType == NODE_TYPE_LOWERBOUND) { 
                alpha = entry.score > alpha ? entry.score : alpha;
            } else {
                if (pliesFromRoot == 0) {
                    ctx->bestMoveThisIteration.col = entry.col;
                    ctx->bestMoveThisIteration.score = entry.score;
                }
                return entry.score;
            }
        }
        
    }

    if (pliesRemaining == 0) { return evaluation(rbb, ybb, heights, sideToMove); }
        
    int origAlpha = alpha;
    BestMove bestMoveInThisPos;
    int cols[7];
    memset(cols, 0, sizeof(cols));

    Moves validMoves = getPossibleCols(heights);
    orderMoves(&validMoves, rbb, ybb, 0);

    for (int i = 0; i < validMoves.count; i++) {
        int col = validMoves.moves[i];
        int index = heights[col] * 7 + col;
        if (sideToMove == RED) {
            index = placePiece(&rbb, heights, col);
            hashVal ^= randVals[index];
            hashVal ^= randVals[ZOBRIST_YELLOW_TO_MOVE];
        } else {
            index = placePiece(&ybb, heights, col);
            hashVal ^= randVals[ZOBRIST_YELLOW_OFFSET + index];
            hashVal ^= randVals[ZOBRIST_RED_TO_MOVE];
        }
        
        int oppSideToMove = (sideToMove == RED) ? YELLOW : RED;
        int eval = -alphaBetaSearch(rbb, ybb, pliesRemaining - 1, pliesFromRoot + 1,
                        -beta, -alpha, heights, oppSideToMove, hashVal, randVals, ctx, tTable);

        if (sideToMove == RED) {
            index = removePiece(&rbb, heights, col);
            hashVal ^= randVals[index];
            hashVal ^= randVals[ZOBRIST_YELLOW_TO_MOVE];
        } else {
            index = removePiece(&ybb, heights, col);
            hashVal ^= randVals[ZOBRIST_YELLOW_OFFSET + index];
            hashVal ^= randVals[ZOBRIST_RED_TO_MOVE];
        }

        time_t current_time = time(NULL);
        if (difftime(current_time, ctx->start_time) >= MAX_TIME) {
            ctx->searchCanceled = true;
            break;
        }

        if (eval > alpha) {
            bestMoveInThisPos.col = col;
            bestMoveInThisPos.score = eval;
            if (pliesFromRoot == 0) {
                ctx->bestMoveThisIteration.col = col;
                ctx->bestMoveThisIteration.score = eval;
            }
            alpha = eval;
        }

        if (alpha >= beta) { break; }
    }

    TTEntry entry;
    entry.key = hashVal;
    entry.col = bestMoveInThisPos.col;
    entry.score = bestMoveInThisPos.score;
    entry.nodeType = setNodeType(alpha, beta, origAlpha);
    entry.depth = pliesFromRoot;
    storeEntry(entry, tTable);

    return alpha;
}

void runIterativeDeepening(bitboard rbb, bitboard ybb, int heights[7], int sideToMove, uint64_t hashVal, uint64_t randVals[86], 
SearchContext *ctx, TTEntry tTable[TT_SIZE]) { 
    ctx->start_time = time(NULL);
    ctx->searchCanceled = false;

    // Iteratively search deeper (maximum 42 moves)
    for (int i = 1; i < 43; i++) {
        alphaBetaSearch(rbb, ybb, i, 0, -INFINITY, INFINITY, heights, sideToMove, hashVal, randVals, ctx, tTable);
        printf("%d ", i);
        
        time_t current_time = time(NULL);        
        if (difftime(current_time, ctx->start_time) >= MAX_TIME) { ctx->searchCanceled = true; }

        if (ctx->searchCanceled) {
            // Reset cancellation flag for subsequent calls if necessary.
            ctx->searchCanceled = false;
            return;
        } else {
            ctx->bestMove.col = ctx->bestMoveThisIteration.col;
            ctx->bestMove.score = ctx->bestMoveThisIteration.score;
            // Return early if forced win/loss is found
            if (ctx->bestMove.score == LOST || ctx->bestMove.score == WON) { return; }
        }
    }   
}

int search(bitboard rbb, bitboard ybb, int heights[7], int sideToMove, uint64_t hashVal, uint64_t randVals[86], TTEntry tTable[TT_SIZE]) {

    SearchContext ctx;
    ctx.bestMove.col = -1;
    ctx.bestMove.score = -INFINITY;
    ctx.bestMoveThisIteration.col = -1;
    ctx.bestMoveThisIteration.score = -INFINITY;

    runIterativeDeepening(rbb, ybb, heights, sideToMove, hashVal, randVals, &ctx, tTable);
    printf("best score: %d ", ctx.bestMove.score);
    printf("best col: %d", ctx.bestMove.col);
    
    return ctx.bestMove.col;
}
