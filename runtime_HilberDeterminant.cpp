#include <cmath>
#include <iostream>
#include <array>
#include <chrono>
#include "shadow_memory.hpp"
#include "tmp_mta_space.hpp"
using namespace std;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
//---------------------------------------------------------------------------------------
// DETERMINANT OF A HILBERT MATRIX

/*
 * determinant of Hilbert's matrix
 *
 * The gradual loss of accuracy is pointed out by CADNA.
 * One can see that the value of the determinant is significant even if it is very "small".
 * This shows how difficult it is to judge the numerical quality of a computed result by its magnitude.
 *
 * http://www-pequan.lip6.fr/cadna/Examples_Dir/ex3.php
 */

int main() {
  cout << "The determinant of Hilbert's matrix" << '\n'
            << "The gradual loss of accuracy is pointed out." << '\n'
            << "One can see that the value of the determinant is significant even if it is very \"small\"." << endl;

  // array<array<double, 11>, 11> amat;
  // for (int i = 1; i <= 11; i++) {
  //   for (int j = 1; j <= 11; j++) {
  //     amat[i - 1][j - 1] = 1.0 / (i + j - 1);
  //   }
  // }
  auto start_time = chrono::high_resolution_clock::now();
  float amat[11][11];
  for (int i = 1; i <= 11; i++) {
    for (int j = 0; j < 11; j++) {
      amat[i - 1][j - 1] = 1.0 / (i - 1 + j - 1 + 1);
    }
  }
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 11; j++) {
      tmpctx->t_store(&amat[i][j], tmpctx->t_const(amat[i][j], SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    }
  }
  // float det = 1.0;
  float det = 1.0;
  tmpctx->t_store(&det, tmpctx->t_const(det, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
  for (int i = 0; i < 10; i++) {
    float pivot = amat[i][i];
    tmpctx->t_store(&pivot, tmpctx->t_const(pivot, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    cout << "Pivot float " << i << " = " << pivot << endl;
    det = det * pivot;
    fp_entry* tdet = tmpctx->t_load(&det, SITE_ID(), __LINE__);
    fp_entry* tpiv = tmpctx->t_load(&pivot, SITE_ID(), __LINE__);
    fp_entry* tprod = tmpctx->t_mul(tdet, tpiv, SITE_ID(), __LINE__);
    tmpctx->t_store(&det, tprod, SITE_ID(), __LINE__);

    float div = 1.0;
    float aux = div / pivot;
    fp_entry* taux = tmpctx->t_div(tmpctx->t_const(div, SITE_ID(), __LINE__), tpiv, SITE_ID(), __LINE__);
    for (int j = i + 1; j < 11; j++) {
      amat[i][j] = amat[i][j] * aux;
      fp_entry* taij = tmpctx->t_load(&amat[i][j], SITE_ID(), __LINE__);
      fp_entry * tmul = tmpctx->t_mul(taij, taux, SITE_ID(), __LINE__);
      tmpctx->t_store(&amat[i][j], tmul, SITE_ID(), __LINE__);
    }

    for (int j = i + 1; j < 11; j++) {
      aux = amat[j][i];
      fp_entry* taux = tmpctx->t_load(&amat[j][i], SITE_ID(), __LINE__);
      for (int k = i + 1; k < 11; k++) {
        amat[j][k] = amat[j][k] - aux * amat[i][k];
        fp_entry* taik = tmpctx->t_load(&amat[i][k], SITE_ID(), __LINE__);
        fp_entry* tajk = tmpctx->t_load(&amat[j][k], SITE_ID(), __LINE__);
        fp_entry* tmul = tmpctx->t_mul(taux, taik, SITE_ID(), __LINE__);
        fp_entry* tsub = tmpctx->t_sub(tajk, tmul, SITE_ID(), __LINE__);
        tmpctx->t_store(&amat[j][k], tsub, SITE_ID(), __LINE__);
      }
    }
  }
  float pivot = amat[10][10];
  tmpctx->t_store(&pivot, tmpctx->t_const(amat[10][10], SITE_ID(), __LINE__), SITE_ID(), __LINE__);
  det = det * pivot;
  fp_entry* tdet = tmpctx->t_load(&det, SITE_ID(), __LINE__);
  fp_entry* tpiv = tmpctx->t_load(&pivot, SITE_ID(), __LINE__);
  fp_entry* tprod = tmpctx->t_mul(tdet, tpiv, SITE_ID(), __LINE__);
  tmpctx->t_store(&det, tprod, SITE_ID(), __LINE__);
  cout << "Pivot " << 10 << " = " << pivot << endl;
  cout << "Determinant = " << det << endl;

  // int i;
  // for (i = 1; i <= 10; i++)
  // {
  //   float pivot = amat[i - 1][i - 1];
  //   cout << "Pivot double " << i << " = " << pivot << endl;
  //   det = det * pivot;

  //   double aux = 1.0 / pivot;
  //   for (int j = i + 1; j <= 11; j++)
  //   {
  //     amat[i-1][j-1] = amat[i-1][j-1] * aux;
  //   }

  //   for (int j = i + 1; j <= 11; j++)
  //   {
  //     aux = amat[j-1][i-1];
  //     for (int k = i + 1; k<= 11; k++)
  //     {
  //       amat[j - 1][k - 1] = amat[j - 1][k - 1]  -  aux*amat[i - 1][k - 1];
  //     }
  //   }
  // }

  // double pivot = amat[i - 1][i - 1];
  // det = det * pivot;
  // cout << "Pivot double " << i << " = " << pivot << endl;
  // cout << "Determinant = " << det << endl;
}