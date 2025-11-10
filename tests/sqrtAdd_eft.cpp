#include <iostream>
#include <eft/eft.hpp>
using namespace std;
using namespace eft;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
// #define TRUE 1
// #define FALSE 0

double ex0(double x) {
	return 1.0 / (sqrt((x + 1.0)) + sqrt(x));
}

int main() {
	// ex0(2.5);
	double x = 2.5;
	// double o = 1;
	tmpctx->t_store(&x, tmpctx->t_const(x, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* tx = tmpctx->t_load(&x, SITE_ID(), __LINE__);
	fp_entry* tsqrt = tmpctx->t_sqrt(tx, SITE_ID(), __LINE__);
	fp_entry* tadd = tmpctx->t_add(tsqrt, tmpctx->t_sqrt(tmpctx->t_add(tx, tmpctx->t_const(1, SITE_ID(), __LINE__), SITE_ID(), __LINE__), SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* tdiv = tmpctx->t_div(tmpctx->t_const(1, SITE_ID(), __LINE__), tadd, SITE_ID(), __LINE__);
	cout << "org value: " << 1.0 / (sqrt((x + 1.0)) + sqrt(x)) << endl;
    tmpctx->dump_tracing(tdiv);
    tmpctx->dump_sum();
}