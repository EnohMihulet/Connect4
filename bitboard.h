#pragma once 
#include "helper.h"


void setBit(bitboard *bb, int pos) {
    *bb |= (1ULL << pos); 
}

void clearBit(bitboard *bb, int pos) {
    *bb &= ~(1ULL << pos);
}

int getBit(bitboard bb, int pos) {
    return (bb >> pos) & 1;
}

void printBitboard(bitboard bb) {
    const int rows = 6, cols = 7;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int index = row * cols + col;
            printf("%d ", getBit(bb, index));
        }
        printf("\n");
    }
    printf("\n");
}

int placePiece(bitboard *bb, int heights[7], int col) {
    if (heights[col] >= 6)
        return -1;
    int row = 5 - heights[col];
    int index = row * 7 + col;
    setBit(bb, index);
    heights[col]++;
    return index;
}

int removePiece(bitboard *bb, int heights[7], int col) {
    int row = 5 - (heights[col] - 1);
    int index = row * 7 + col;
    clearBit(bb, index);
    heights[col]--;
    return index;
}

Moves getPossibleCols(const int heights[7]) {
     Moves valid; 
     valid.count = 0; 
     for (int i = 0; i < 7; i++) { 
        int col = i; 
        if (heights[col] < 6) { 
            valid.moves[valid.count++] = col; 
        } 
    } 
    return valid; 
}

int popcount(bitboard bb) {
    return __builtin_popcountll(bb);
}

int is4InARow(bitboard bb) {
    // 3 Bitboard for horizontal 4 in row check
    bitboard hbb1 = (bb & FIRST_COLUMN_MASK) >> 1;
    bitboard hbb2 = (hbb1 & FIRST_COLUMN_MASK) >> 1;
    bitboard hbb3 = (hbb2 & FIRST_COLUMN_MASK) >> 1;

    // 3 bitboards for vertical 4 in row check
    bitboard vbb1 = bb >> 7;
    bitboard vbb2 = bb >> 14;
    bitboard vbb3 = bb >> 21;

    // 3 bitboards for forward diagonal 4 in row check
    bitboard fdbb1 = (bb & FIRST_COLUMN_MASK) >> 8;
    bitboard fdbb2 = (fdbb1 & FIRST_COLUMN_MASK) >> 8;
    bitboard fdbb3 = (fdbb2 & FIRST_COLUMN_MASK) >> 8;

    // 3 bitboards for backward diagonal 4 in row check
    bitboard bdbb1 = (bb & SEVENTH_COLUMN_MASK) >> 6;
    bitboard bdbb2 = (bdbb1 & SEVENTH_COLUMN_MASK) >> 6;
    bitboard bdbb3 = (bdbb2 & SEVENTH_COLUMN_MASK) >> 6;
    
    if ((bb & hbb1 & hbb2 & hbb3) > 0) {
        return 1;
    } else if ((bb & vbb1 & vbb2 & vbb3) > 0) {
        return 1;
    } else if ((bb & fdbb1 & fdbb2 & fdbb3) > 0) {
        return 1;
    } else if ((bb & bdbb1 & bdbb2 & bdbb3) > 0) {
        return 1;
    }
    return -1;
}

int fourInARowCount(bitboard bb) {
    int count = 0;
    bb = ~bb;

    bitboard hbb1 = (bb & FIRST_COLUMN_MASK) >> 1;
    bitboard hbb2 = (hbb1 & FIRST_COLUMN_MASK) >> 1;
    bitboard hbb3 = (hbb2 & FIRST_COLUMN_MASK) >> 1;

    bitboard vbb1 = bb >> 7;
    bitboard vbb2 = bb >> 14;
    bitboard vbb3 = bb >> 21;

    bitboard fdbb1 = (bb & FIRST_COLUMN_MASK) >> 8;
    bitboard fdbb2 = (fdbb1 & FIRST_COLUMN_MASK) >> 8;
    bitboard fdbb3 = (fdbb2 & FIRST_COLUMN_MASK) >> 8;

    bitboard bdbb1 = (bb & SEVENTH_COLUMN_MASK) >> 6;
    bitboard bdbb2 = (bdbb1 & SEVENTH_COLUMN_MASK) >> 6;
    bitboard bdbb3 = (bdbb2 & SEVENTH_COLUMN_MASK) >> 6;
    
    count += popcount(bb & hbb1 & hbb2 & hbb3);
    count += popcount(bb & vbb1 & vbb2 & vbb3);
    count += popcount(bb & fdbb1 & fdbb2 & fdbb3);
    count += popcount(bb & bdbb1 & bdbb2 & bdbb3);
    return count;
}

// int ThreeInARowCount(bitboard)

int printBoard(bitboard rbb, bitboard ybb) {
    printf("\n   1   2   3   4   5   6   7   ");
    for (int i = 0; i < 42; i++) {
        if (i % 7 == 0) {
            if (i != 0) {
                printf(" |");
            }
            printf("\n");
            printf(" +---+---+---+---+---+---+---+\n");
        }
        if (getBit(rbb, i) == 1) {
            printf(" | \033[31m●\033[0m");
        } else if (getBit(ybb, i) == 1) {
            printf(" | \033[33m●\033[0m");
        } else {
            printf(" |  ");
        }
    }
    printf(" |\n +---+---+---+---+---+---+---+\n");
    return 1;
}

int gameOver(bitboard rbb, bitboard ybb) {
    if (is4InARow(rbb) == 1) { return RED; } 
    else if (is4InARow(ybb) == 1) { return YELLOW; } 
    else if ((rbb | ybb) == FULL_BOARD) { return 0; }
    return -1;
}