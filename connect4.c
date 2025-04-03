#include <errno.h> 
#include <limits.h> 
#include <stdbool.h> 
#include <stdint.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "helper.h" 
#include "bitboard.h" 
#include "zobrist.h" 
#include "search.h"


TTEntry tTable[TT_SIZE];


int main(void) {
    int col;
    bitboard rbb = 0;
    bitboard ybb = 0;

    u_int64_t randVals[86];
    zobristInit(randVals);

    int heights[7];
    memset(heights, 0, sizeof(heights));
   
    int sideToMove = RED;
    int playerColor = RED;
    int mode = 1;
    int index;

    while (true) {
        rbb = 0;
        ybb = 0;

        memset(heights, 0, sizeof(heights));

        sideToMove = RED;
        uint64_t hashVal = randVals[ZOBRIST_RED_TO_MOVE];

        for (int i = 0; i < TT_SIZE; i++) {
            tTable[i].col = -1;
        }   
        
        printf("Enter 1 for Player vs Player. Enter 2 to play against a bot. ");
        while (true) {
            mode = getInput();
            if (mode == 1 || mode == 2) {
                break;
            }
            printf("Invalid input. Try Again.\n");
        }

        if (mode == 2) {
            printf("Enter 1 to play as red. Enter 2 to play as yellow. ");
            while (true) {
                playerColor = getInput();
                if (playerColor == 1 || playerColor == 2) {
                    break;   
                }
                printf("Invalid input. Try Again.");
            }
        }
        
        printBoard(rbb, ybb);

        while (true) {
            if (sideToMove == playerColor || mode == 1) {
                printf("Enter a column: ");
                col = getInput();
                if (col < 1 || col > 7) {
                    fprintf(stderr, "Error: Column out of range.\n");
                    continue;
                }
            }
        
            if (mode == 1) {
                if (sideToMove == RED) {
                    index = placePiece(&rbb, heights, col - 1);
                    if (index == -1) {
                        printf("Column full. Select a different column.\n");
                        continue;
                    }
                } else if (sideToMove == YELLOW) {
                    index = placePiece(&ybb, heights, col - 1);
                    if (index == -1) {
                        printf("Column full. Select a different column.\n");
                        continue;
                    }
                }
            } else if (mode == 2) {
                if (sideToMove == playerColor) {
                    if (playerColor == RED) {
                        index = placePiece(&rbb, heights, col - 1);
                        if (index == -1) {
                            printf("Column full. Select a different column.\n");
                            continue;
                        } 
                    } else {
                        index = placePiece(&ybb, heights, col - 1);
                        if (index == -1) {
                            printf("Column full. Select a different column.\n");
                            continue;
                        }
                    }
                } else {
                    col = search(rbb, ybb, heights, sideToMove, hashVal, randVals, tTable);
                    if (sideToMove == RED) {
                        index = placePiece(&rbb, heights, col);
                    } else {
                        index = placePiece(&ybb, heights, col);
                    }
                }
            }
            printBoard(rbb, ybb);

            int gameState = gameOver(rbb, ybb);
            if (gameState == -1) {
                // Update zobrist hash
                if (sideToMove == RED) {
                    hashVal ^= randVals[index];
                    hashVal ^= randVals[ZOBRIST_YELLOW_TO_MOVE];
                } else {
                    hashVal ^= randVals[ZOBRIST_YELLOW_OFFSET + index];
                    hashVal ^= randVals[ZOBRIST_RED_TO_MOVE];
                }
                switchSide(&sideToMove);
                continue;
            }

            if (gameState == 0) {
                printf("DRAW\n");
            } else if (gameState == RED) {
                printf("RED wins\n");
            } else {
                printf("YELLOW wins\n");
            }

            printf("To play again, enter 1. To exit, enter any other number. ");
            if (getInput() == 1) {
                break;
            } else {
                return 1;
            }
        }  
    }
}