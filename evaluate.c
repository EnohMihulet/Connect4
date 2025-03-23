#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "bitboard.c"


const int POSSIBLE_FOUR_IN_ROW = 10;


int evaluation(bitboard rbb, bitboard ybb, int heights[7], int sideToMove) {
    int sign = sideToMove == RED ? 1 : -1;
    return sign * POSSIBLE_FOUR_IN_ROW * (fourInARowCount(rbb) - fourInARowCount(ybb));
}