#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 3
#define COLS 3

int main() {
    float array[ROWS][COLS];

    srand(time(NULL));
    printf("\n{");
    for (int i = 0; i < ROWS; ++i) {
        printf("{");
        for (int j = 0; j < COLS; ++j) {
            // Generate random value between -1 and 1
            array[i][j] = ((float)rand() / RAND_MAX) * 2 - 1;
            printf("%.2f", array[i][j]);
            if (j < COLS - 1) {
                printf(", ");
            }
        }
        printf("}");
        if (i < ROWS - 1) {
            printf(",");
        }
    }
    printf("},\n");
    printf("\n");
    return 0;
}
