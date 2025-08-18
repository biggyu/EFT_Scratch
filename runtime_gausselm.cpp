// #include <stdio.h>
// #include <stdlib.h>

#include "shadow_memory.hpp"
using namespace std;

// ShadowMemory smem;
ShadowMemory* smem = new ShadowMemory();
#define SMEM_STORE(ptr) \
    smem->on_store((void*)(ptr), static_cast<double>(*(ptr)), fp_op::STORE, __LINE__, nullptr, nullptr);
#define SMEM_LOAD(ptr, value) \
    smem->on_load((void*)(ptr), static_cast<double>(value), fp_op::LOAD, __LINE__);
#define SMEM_DUMP() \
    smem->dump_summary("ShadowMemory Summary");

int main() {
    int dim = 3;

    // // Random Values for A, x
    // float A[dim][dim];
    // float x[dim];
    // srand((unsigned int)time(NULL));
    // for(int i = 0; i < dim; i++) {
    //     for(int j = 0; j < dim; j++) {
    //         A[i][j] = (rand() / (float)RAND_MAX + rand() % 15) * (rand() % 2 == 1 ? 1 : -1);
    //     }
    //     x[i] = (rand() / (float)RAND_MAX + rand() % 20) * (rand() % 2 == 1 ? 1 : -1);
    // }

    // Initialize Matrix, Vector
    float A[3][3] = {
        {-16.1598, 15.7984, 9.3352,},
        {-6.4460, 3.3648, -5.0838,},
        {11.1416, -8.7571, 2.1567,},
    };
    float x[3] = {-14.8912, -1.4871, -6.7040};
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            SMEM_STORE(&A[i][j]);
        }
        SMEM_STORE(&x[i]);
    }
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
            float Aji, Aii;
            SMEM_LOAD(&A[j][i], Aji);
            SMEM_LOAD(&A[i][i], Aii);
            float ratio = Aji / Aii;

            for(int k = 0; k < dim; k++) {
                float Ajk, Aik;
                SMEM_LOAD(&A[j][k], Ajk);
                SMEM_LOAD(&A[i][k], Aik);
                A[j][k] = Ajk - ratio * Aik;
                SMEM_STORE(&A[j][k]);
            }
            float xj, xi;
            SMEM_LOAD(&x[j], xj);
            SMEM_LOAD(&x[i], xi);
            x[j] = xj - ratio * xi;
            SMEM_STORE(&x[j]);
        }
    }

    // Reduced Echelon Form
    for(int i = dim; i > 0 ; i--) {
        for(int j = i - 1; j > 0; j--) {
            float Aji, Aii;
            SMEM_LOAD(&A[j - 1][i - 1], Aji);
            SMEM_LOAD(&A[i - 1][i - 1], Aii);
            float ratio = Aji / Aii;

            for(int k = 0; k < dim; k++) {
                float Ajk, Aik;
                SMEM_LOAD(&A[j - 1][k], Ajk);
                SMEM_LOAD(&A[i - 1][k], Aik);
                A[j - 1][k] = Ajk - ratio * Aik;
                SMEM_STORE(&A[j - 1][k]);
            }
            float xj, xi;
            SMEM_LOAD(&x[j - 1], xj);
            SMEM_LOAD(&x[i - 1], xi);
            x[j - 1] = xj - ratio * xi;
            SMEM_STORE(&x[j - 1]);
        }
    }
    for(int i = 0; i < dim; i++) {
        float xi, Aii;
        SMEM_LOAD(&x[i], xi);
        SMEM_LOAD(&A[i][i], Aii);
        x[i] = xi / Aii;
        SMEM_STORE(&x[i]);
        A[i][i] = 1.0f;
        SMEM_STORE(&A[i][i]);
    }

    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\t%f\n", x[i]);
        // printf("\n");
    }
    SMEM_DUMP();
    return 0;
}