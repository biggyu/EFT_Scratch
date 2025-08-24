// #include <stdio.h>
// #include <stdlib.h>

#include "shadow_memory.hpp"
#include "tmp_mta_space.hpp"
using namespace std;

// ShadowMemory smem;
ShadowMemory* smem = new ShadowMemory();
TempContext tmpctx(smem, 256);

template<typename T>
void SMemStore(T* ptr, ShadowMemory* smem, size_t linenum) {
    smem->on_store((void*)ptr, *(ptr), fp_op::STORE, __LINE__, nullptr, nullptr);
    // smem->on_store((void*)ptr, *(ptr), fp_op::STORE, linenum);
}

template <typename T>
T SMemLoad(T* ptr, ShadowMemory* smem, size_t linenum) {
  T v = *ptr;
  smem->on_load((void*)ptr, (double)v, fp_op::LOAD, linenum);
  return v;
}

void SMemDump() {
    smem->dump_summary();
}

int main() {
    // float A00 = 2.0f;
    // float t = 0.0f;
    // SMEM_STORE(&A00);
    // SMEM_LOAD(&A00, t);
    // printf("t = %f\n", t); 

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
            // // SMEM_STORE(&A[i][j]);
            // SMemStore(&A[i][j], smem, __LINE__);
            t_store(&A[i][j], t_const(&A[i][j], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
            // t_store(&A[i][j], t_const(A[i][j], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
        }
        // // SMEM_STORE(&x[i]);
        // SMemStore(&x[i], smem, __LINE__);
        t_store(&x[i], t_const(&x[i], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
        // t_store(&x[i], t_const(x[i], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
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
            // // float Aji, Aii;
            // // SMEM_LOAD(&A[j][i], Aji);
            // // SMEM_LOAD(&A[i][i], Aii);
            // float Aji = SMemLoad(&A[j][i], smem, __LINE__);
            // float Aii = SMemLoad(&A[i][i], smem, __LINE__);
            // float ratio = Aji / Aii;
            float ratio = A[j][i] / A[i][i];
            fp_entry* tAji = t_load(&A[j][i], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tAii = t_load(&A[i][i], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tratio = t_div(tAji, tAii, tmpctx, SITE_ID(), __LINE__);

            for(int k = 0; k < dim; k++) {
                // // float Ajk, Aik;
                // // SMEM_LOAD(&A[j][k], Ajk);
                // // SMEM_LOAD(&A[i][k], Aik);
                // float Ajk = SMemLoad(&A[j][k], smem, __LINE__);
                // float Aik = SMemLoad(&A[i][k], smem, __LINE__);
                // A[j][k] = Ajk - ratio * Aik;
                // // SMEM_STORE(&A[j][k]);
                // SMemStore(&A[j][k], smem, __LINE__);
                A[j][k] = A[j][k] - ratio * A[i][k];
                fp_entry* tAjk = t_load(&A[j][k], tmpctx, SITE_ID(), __LINE__);
                fp_entry* tAik = t_load(&A[i][k], tmpctx, SITE_ID(), __LINE__);
                fp_entry* tprod = t_mul(tratio, tAik, tmpctx, SITE_ID(), __LINE__);
                fp_entry* tsub = t_sub(tAjk, tprod, tmpctx, SITE_ID(), __LINE__);
                t_store(&A[j][k], tsub, tmpctx, SITE_ID(), __LINE__);
            }
            // // float xj, xi;
            // // SMEM_LOAD(&x[j], xj);
            // // SMEM_LOAD(&x[i], xi);
            // float xj = SMemLoad(&x[j], smem, __LINE__);
            // float xi = SMemLoad(&x[i], smem, __LINE__);
            // x[j] = xj - ratio * xi;
            // // SMEM_STORE(&x[j]);
            // SMemStore(&x[j], smem, __LINE__);
            x[j] = x[j] - ratio * x[i];
            fp_entry* txj = t_load(&x[j], tmpctx, SITE_ID(), __LINE__);
            fp_entry* txi = t_load(&x[i], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tprod = t_mul(tratio, txi, tmpctx, SITE_ID(), __LINE__);
            fp_entry* tsub = t_sub(txj, tprod, tmpctx, SITE_ID(), __LINE__);
            t_store(&x[j], tsub, tmpctx, SITE_ID(), __LINE__);
        }
    }

    // Reduced Echelon Form
    for(int i = dim; i > 0 ; i--) {
        for(int j = i - 1; j > 0; j--) {
            // // float Aji, Aii;
            // // SMEM_LOAD(&A[j - 1][i - 1], Aji);
            // // SMEM_LOAD(&A[i - 1][i - 1], Aii);
            // float Aji = SMemLoad(&A[j - 1][i - 1], smem, __LINE__);
            // float Aii = SMemLoad(&A[i - 1][i - 1], smem, __LINE__);
            // float ratio = Aji / Aii;
            float ratio = A[j - 1][i - 1] / A[i - 1][i - 1];
            fp_entry* tAji = t_load(&A[j - 1][i - 1], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tAii = t_load(&A[i - 1][i - 1], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tratio = t_div(tAji, tAii, tmpctx, SITE_ID(), __LINE__);

            for(int k = 0; k < dim; k++) {
                // // float Ajk, Aik;
                // // SMEM_LOAD(&A[j - 1][k], Ajk);
                // // SMEM_LOAD(&A[i - 1][k], Aik);
                // float Ajk = SMemLoad(&A[j - 1][k], smem, __LINE__);
                // float Aik = SMemLoad(&A[i - 1][k], smem, __LINE__);
                // A[j - 1][k] = Ajk - ratio * Aik;
                // // SMEM_STORE(&A[j - 1][k]);
                // SMemStore(&A[j - 1][k], smem, __LINE__);
                A[j - 1][k] = A[j - 1][k] - ratio * A[i - 1][k];
                fp_entry* tAjk = t_load(&A[j - 1][k], tmpctx, SITE_ID(), __LINE__);
                fp_entry* tAik = t_load(&A[i - 1][k], tmpctx, SITE_ID(), __LINE__);
                fp_entry* tprod = t_mul(tratio, tAik, tmpctx, SITE_ID(), __LINE__);
                fp_entry* tresult = t_sub(tAjk, tprod, tmpctx, SITE_ID(), __LINE__);
                t_store(&A[j - 1][k], tresult, tmpctx, SITE_ID(), __LINE__);
            }
            // // float xj, xi;
            // // SMEM_LOAD(&x[j - 1], xj);
            // // SMEM_LOAD(&x[i - 1], xi);
            // float xj = SMemLoad(&x[j - 1], smem, __LINE__);
            // float xi = SMemLoad(&x[i - 1], smem, __LINE__);
            // x[j - 1] = xj - ratio * xi;
            // // SMEM_STORE(&x[j - 1]);
            // SMemStore(&x[j - 1], smem, __LINE__);
            x[j - 1] = x[j - 1] - ratio * x[i - 1];
            fp_entry* txj = t_load(&x[j - 1], tmpctx, SITE_ID(), __LINE__);
            fp_entry* txi = t_load(&x[i - 1], tmpctx, SITE_ID(), __LINE__);
            fp_entry* tprod = t_mul(tratio, txi, tmpctx, SITE_ID(), __LINE__);
            fp_entry* tresult = t_sub(txj, tprod, tmpctx, SITE_ID(), __LINE__);
            t_store(&x[j - 1], tresult, tmpctx, SITE_ID(), __LINE__);
        }
    }
    for(int i = 0; i < dim; i++) {
        // // float xi, Aii;
        // // SMEM_LOAD(&x[i], xi);
        // // SMEM_LOAD(&A[i][i], Aii);
        // float xi = SMemLoad(&x[i], smem, __LINE__);
        // float Aii = SMemLoad(&A[i][i], smem, __LINE__);
        // x[i] = xi / Aii;
        // // SMEM_STORE(&x[i]);
        // SMemStore(&x[i], smem, __LINE__);
        // // SMEM_STORE(&A[i][i]);
        // SMemStore(&A[i][i], smem, __LINE__);
        x[i] = x[i] / A[i][i];
        fp_entry* txi = t_load(&x[i], tmpctx, SITE_ID(), __LINE__);
        fp_entry* tAii = t_load(&A[i][i], tmpctx, SITE_ID(), __LINE__);
        fp_entry* tdiv = t_div(txi, tAii, tmpctx, SITE_ID(), __LINE__);
        t_store(&x[i], tdiv, tmpctx, SITE_ID(), __LINE__);

        A[i][i] = 1.0;
        t_store(&A[i][i], t_const(&A[i][i], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
        // t_store(&A[i][i], t_const(A[i][i], tmpctx, SITE_ID(), __LINE__), tmpctx, SITE_ID(), __LINE__);
    }

    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%f ", A[i][j]);
        }
        printf("\t%f\n", x[i]);
        // printf("\n");
    }
    // SMemDump();
    return 0;
}