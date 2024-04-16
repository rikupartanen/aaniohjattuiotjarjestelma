#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    
    const int ROWS = 124;
    const int COLS = 129;
    int array[ROWS][COLS];

    // Täytetään array arvoilla 0-255
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            array[i][j] = rand() % 256;
        }
    }
    // Printataan array
    printf("{\n");
    for (int i = 0; i < ROWS; i++) {
        printf("    {");
        for (int j = 0; j < COLS; j++) {
            printf("%d", array[i][j]);
            if (j < COLS - 1) {
                printf(", ");
            }
        }
        printf("}");
        if (i < ROWS - 1) {
            printf(",");
        }
        printf("\n");
    }
    printf("}\n");
    return 0;
}