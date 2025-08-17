#include <stdio.h>
#include <stdlib.h>
void main() {
    int dim = 4;
    // int **A;
    // A = (int **) malloc (sizeof(int*) * dim);
    // // for(int i = 0; i < dim; i++) {
    // //     A[i] = (int*) malloc ( sizeof(int) * dim);
    // // }
    // *A[0] = {2, 1, -1};
    // *A[1] = {-3, -1, 2};
    // *A[2] = {-2, 1, 2};
    // for(int i = 0; i < 5; i++) {
    //     printf("%.3f ", rand() / (float)RAND_MAX + (rand() % 20 > 9 ? rand() % 20 : rand() % 20 * -1));
    // }
    // printf("\n");

    // Random Values for A, x
    float A[dim][dim];
    float x[dim];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            A[i][j] = (rand() / (float)RAND_MAX + rand() % 15) * (rand() % 2 == 1 ? 1 : -1);
        }
        x[i] = (rand() / (float)RAND_MAX + rand() % 20) * (rand() % 2 == 1 ? 1 : -1);
    }

    // // Initialize Matrix, Vector
    // float A[3][3] = {
    //     {-16.1598, 15.7984, 9.3352,},
    //     {-6.4460, 3.3648, -5.0838,},
    //     {11.1416, -8.7571, 2.1567,},
    // };
    // float x[3] = {-14.8912, -1.4871, -6.7040};
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\t%f\n", x[i]);
    }
    printf("\n");

    // Echelon Form
    for(int i = 0; i < dim; i++) {
        for(int j = i + 1; j < dim; j++) {
            float ratio = A[j][i] * 1.0 / A[i][i];
            // printf("ratio: %f\n", ratio);
            for(int k = 0; k < dim; k++) {
                A[j][k] -= ratio * A[i][k];
            }
            x[j] -= ratio * x[i];
        }
    }

    // Reduced Echelon Form
    for(int i = dim; i > 0 ; i--) {
        for(int j = i - 1; j > 0; j--) {
            float ratio = A[j - 1][i - 1] * 1.0 / A[i - 1][i - 1];
            // printf("ratio: %f\n", ratio);
            for(int k = 0; k < dim; k++) {
                A[j - 1][k] -= ratio * A[i - 1][k];
            }
            x[j - 1] -= ratio * x[i - 1];
        }
    }
    for(int i = 0; i < dim; i++) {
        x[i] /= A[i][i];
        A[i][i] /= A[i][i]; //?
        // A[i][i] = 1.0; //? 
    }

    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\t%f\n", x[i]);
        // printf("\n");
    }
}