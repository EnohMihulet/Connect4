#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>


typedef enum {EMPTY, RED, YELLOW} PieceColor;
typedef uint64_t bitboard;

const bitboard SEVENTH_COLUMN_MASK = ~((1ULL << 6) | (1ULL << 13) | (1ULL << 20) | (1ULL << 27) | (1ULL << 34) | (1ULL << 41));
const bitboard FIRST_COLUMN_MASK = ~((1ULL << 0) | (1ULL << 7) | (1ULL << 14) | (1ULL << 21) | (1ULL << 28) | (1ULL << 35));
const bitboard FULL_BOARD = (1ULL << 42) - 1;


void switchSide(int *sideToMove) {
    *sideToMove = (*sideToMove == YELLOW) ? RED : YELLOW;
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