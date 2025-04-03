 #pragma once 
 #include <stdbool.h> 
 #include <errno.h> 
 #include <limits.h> 
 #include <stdint.h> 
 #include <stdio.h> 
 #include <stdlib.h> 
 #include <string.h>


typedef enum {EMPTY, RED, YELLOW} PieceColor;
typedef uint64_t bitboard;
typedef struct { int moves[7]; int count; } Moves;

// Define board masks as static constants. 
static const bitboard SEVENTH_COLUMN_MASK = ~((1ULL << 6) | (1ULL << 13) | (1ULL << 20) | (1ULL << 27) | (1ULL << 34) | (1ULL << 41)); 
static const bitboard FIRST_COLUMN_MASK = ~((1ULL << 0) | (1ULL << 7) | (1ULL << 14) | (1ULL << 21) | (1ULL << 28) | (1ULL << 35)); 
static const bitboard FULL_BOARD = (1ULL << 42) - 1;

// Define move scores as static constants.
static const int PREVIOUS_BEST_MOVE = 1000;
static const int MIDDLE_COL = 300;
static const int NEXT_TO_MIDDLE_COL = 200;
static const int EDGE_COL = 100;
static const int BLOCKED = -100;


void switchSide(int *sideToMove) {
    *sideToMove = (*sideToMove == YELLOW) ? RED : YELLOW;
}

void swap(Moves *moves, int scores[7], int i, int j) {
    int tempCol = moves->moves[i];
    int tempScore = scores[i];

    moves->moves[i] = moves->moves[j];
    moves->moves[j] = tempCol;

    scores[i] = scores[j];
    scores[j] = tempScore;
}

int partition(Moves *moves, int scores[7], int low, int high) {
    int pivotScore = scores[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (scores[j] > pivotScore) {
            i++;
            swap(moves, scores, i, j);
        }
    }

    swap(moves, scores, i+1, high);
    return i+1;
}

void quickSort(Moves *moves, int scores[7], int low, int high) {
    if (low < high) {
        int pivotIndex = partition(moves, scores, low, high);
        quickSort(moves, scores, low, pivotIndex-1);
        quickSort(moves, scores, pivotIndex+1, high);
    }
}

void scoreMoves(Moves *moves, int scores[7], bitboard rbb, bitboard ybb, int prevBestMove) {
    for (int i = 0; i < moves->count; i++) {
        if (moves->moves[i] == prevBestMove) {
            scores[i] = PREVIOUS_BEST_MOVE;
            continue;
        } else if (moves->moves[i] == 3) {
            scores[i] = MIDDLE_COL;
            continue;
        } else if (moves->moves[i] == 2 || moves->moves[i] == 4) {
            scores[i] = NEXT_TO_MIDDLE_COL;
            continue;
        } else {
            scores[i] = EDGE_COL;
        }
    }
}

void orderMoves(Moves *moves, bitboard rbb, bitboard ybb, int prevBestMove) {
    int scores[7];
    scoreMoves(moves, scores, rbb, ybb, prevBestMove);
    quickSort(moves, scores, 0, moves->count-1);
}

int getInput()
{
    char buffer[256];
    char *endptr;
    long value;

    // Get user input
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        errno = 0;
        value = strtol(buffer, &endptr, 10);

        // Check if no digits were found
        if (buffer == endptr) {
            fprintf(stderr, "Error: No digits were found.\n");
            return -1;
        }
        // Check for range errors
        if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (value > INT_MAX || value < INT_MIN)) {
            fprintf(stderr, "Error: The number is outside of range for an int.\n");
            return -1;
        }
        // Check for extraneous characters
        while (*endptr != '\0' && (*endptr == '\t' || *endptr == '\n' || *endptr == ' ')) {
            endptr++;
        }
        if (*endptr != '\0') {
            fprintf(stderr, "Error: Unexpected characters after the number.\n");
            return -1;
        }
        
    } else {
        fprintf(stderr, "Error: Input failure.\n");
        return -1;
    }
    return (int) value;
}