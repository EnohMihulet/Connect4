#pragma once 
#include <stdint.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h>
// Define the transposition table size as a static constant. 
static const int TT_SIZE = 262144; // 2^18

// Use a prefixed enum for the node types. 
typedef enum { NODE_TYPE_EXACT, NODE_TYPE_LOWERBOUND, NODE_TYPE_UPPERBOUND } NodeType;

// Use a typedef for the transposition table entry. 
typedef struct { uint64_t key; int score; int col; int depth; NodeType nodeType; } TTEntry;

static int getIndex(uint64_t zobrist) {
        return (int)(zobrist & (uint64_t)(TT_SIZE - 1)); 
    }

static void storeEntry(TTEntry x, TTEntry tTable[TT_SIZE]) {
    int index = getIndex(x.key);
    if (tTable[index].col != -1) {
        int oldDepth = tTable[index].depth;
        int newDepth = x.depth;
        if (oldDepth > newDepth) return;
    } 
    tTable[index] = x;
}

static int getEvaluation(uint64_t zobrist, TTEntry tTable[TT_SIZE]) { 
    int index = getIndex(zobrist);
    TTEntry x = tTable[index];
    return (zobrist == x.key) ? x.score : -1;
}

static NodeType setNodeType(int alpha, int beta, int originalAlpha) { 
    if (alpha == originalAlpha) { return NODE_TYPE_UPPERBOUND; }
    else if (alpha >= beta) { return NODE_TYPE_LOWERBOUND; }
    else { return NODE_TYPE_EXACT; }
}