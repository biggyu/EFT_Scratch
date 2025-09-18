#include <chrono>
#include "shadow_memory.hpp"
#include "tmp_mta_space.hpp"
using namespace std;

// ShadowMemory smem;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
//  `extern void eftsan_print_error(double);
// I expected taylor expansions to have trouble, but it looks like they do not!
// Posit is almost as good as float here.
int main( int argc, const char* argv[] ) {
    auto start_time = chrono::high_resolution_clock::now();
    int dim = 7;
    float fx = 0.875f;
    
    float fcoeff[] = {
        1.0,
        -1.6666667163372039794921875e-01,
        8.333333767950534820556640625e-03,
        -1.98412701138295233249664306640625e-04,
        2.755731884462875314056873321533203125e-06,
        -2.505210972003624192439019680023193359375e-08,
        1.605904298429550181026570498943328857421875e-10
    };
    for (int i = 0; i < dim; i++) {
        tmpctx->t_store(&fcoeff[i], tmpctx->t_const(fcoeff[i], SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    }
    tmpctx->t_store(&fx, tmpctx->t_const(fx, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    
    // Compute Taylor's expansion of sin:
    float fx2 = fx * fx;
    fp_entry* tfx = tmpctx->t_load(&fx, SITE_ID(), __LINE__);
    fp_entry* tfx2 = tmpctx->t_mul(tfx, tfx, SITE_ID(), __LINE__);
    float fxaccum = 0.875f;
    tmpctx->t_store(&fxaccum, tmpctx->t_const(fxaccum, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    float fresult = 0;
    tmpctx->t_store(&fresult, tmpctx->t_const(fresult, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
    for (int i = 0; i < 7; i++){
        fresult += fcoeff[i] * fxaccum;
        fxaccum *= fx2;
        fp_entry* tfxaccum = tmpctx->t_load(&fxaccum, SITE_ID(), __LINE__);
        fp_entry* tfi = tmpctx->t_load(&fcoeff[i], SITE_ID(), __LINE__);
        fp_entry* tprod = tmpctx->t_mul(tfi, tfxaccum, SITE_ID(), __LINE__);
        fp_entry* tresult = tmpctx->t_add(tmpctx->t_load(&fresult, SITE_ID(), __LINE__), tfxaccum, SITE_ID(), __LINE__);
        fp_entry* tprod2 = tmpctx->t_mul(tfxaccum, tfx2, SITE_ID(), __LINE__);
        tmpctx->dump_tracing(tresult);
        tmpctx->dump_tracing(tprod2);
    }

    // eftsan_print_error(fresult);
    printf("float    result: %.50e\n", fresult);
    return 0;
}
