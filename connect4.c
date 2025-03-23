#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include "search.c"


int main(void) {
    int col;
    bitboard rbb = 0;
    bitboard ybb = 0;

    int heights[7];
    memset(heights, 0, sizeof(heights));

    int sideToMove = RED;
    int playerColor = RED;
    int mode = 1;

    while (true) {
        rbb = 0;
        ybb = 0;
        memset(heights, 0, sizeof(heights));
        sideToMove = RED;
        
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
                if (sideToMove == RED && placePiece(&rbb, heights, col - 1) == -1) {
                    printf("Column full. Select a different column.\n");
                    continue;
                } else if (sideToMove == YELLOW && placePiece(&ybb, heights, col - 1) == -1) {
                    printf("Column full. Select a different column.\n");
                    continue;
                }
            } else if (mode == 2) {
                if (sideToMove == playerColor) {
                    if (playerColor == RED) {
                        if (placePiece(&rbb, heights, col - 1) == -1) {
                            printf("Column full. Select a different column.\n");
                            continue;
                        } 
                    } else if (placePiece(&ybb, heights, col - 1) == -1) {
                        printf("Column full. Select a different column.\n");
                        continue;
                    }
                } else {
                    col = search(rbb, ybb, heights, sideToMove);
                    if (sideToMove == RED) {
                        placePiece(&rbb, heights, col);
                    } else {
                        placePiece(&ybb, heights, col);
                    }
                    printf("%d", heights[col]);
                    printf("%d", col);
                }
            }
            printBoard(rbb, ybb);
            // printf("TWO: %d, %d\n", TwoInARowCount(rbb), TwoInARowCount(ybb));
            // printf("THREE: %d, %d\n", ThreeInARowCount(rbb), ThreeInARowCount(ybb));
            // printf("SQUARE: %d, %d\n", SquaresCount(rbb), SquaresCount(ybb));

            int gameState = gameOver(rbb, ybb);
            if (gameState == -1) {
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