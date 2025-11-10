#include <iostream>
#include <eft/eft.hpp>
using namespace std;
using namespace eft;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
#define TRUE 1
#define FALSE 0

double ex0(double x1, double x2) {
	double t = (((3.0 * x1) * x1) + (2.0 * x2)) - x1;
	double t_42_ = (((3.0 * x1) * x1) - (2.0 * x2)) - x1;
	double d = (x1 * x1) + 1.0;
	double s = t / d;
	double s_42_ = t_42_ / d;
	return x1 + (((((((((2.0 * x1) * s) * (s - 3.0)) + ((x1 * x1) * ((4.0 * s) - 6.0))) * d) + (((3.0 * x1) * x1) * s)) + ((x1 * x1) * x1)) + x1) + (3.0 * s_42_));
}

int main() {
	// ex0(1, 2);
	double a = 1;
	double b = 2;
	// tmpctx->t_store(&a, tmpctx->t_const(a, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	// tmpctx->t_store(&b, tmpctx->t_const(b, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	T_STORE(&a, T_CONST(a));
	T_STORE(&b, T_CONST(b));
	// fp_entry* ta = tmpctx->t_load(&a, SITE_ID(), __LINE__);
	// fp_entry* tb = tmpctx->t_load(&b, SITE_ID(), __LINE__);
	fp_entry* ta = T_LOAD(&a);
	fp_entry* tb = T_LOAD(&b);
	// fp_entry* ta1 = tmpctx->t_mul(tmpctx->t_mul(tmpctx->t_const(3.0, SITE_ID(), __LINE__), ta, SITE_ID(), __LINE__), ta, SITE_ID(), __LINE__);
	// fp_entry* tb1 = tmpctx->t_mul(tmpctx->t_const(2.0, SITE_ID(), __LINE__), tb, SITE_ID(), __LINE__);
	fp_entry* ta1 = T_MUL(T_MUL(T_CONST(3.0), ta), ta);
	fp_entry* tb1 = T_MUL(T_CONST(2.0), tb);

	// fp_entry* tt = tmpctx->t_sub(tmpctx->t_add(ta1, tb1, SITE_ID(), __LINE__), ta, SITE_ID(), __LINE__);
	fp_entry* tt = T_SUB(T_ADD(ta1, tb1), ta);

	// fp_entry* tt42 = tmpctx->t_sub(tmpctx->t_sub(ta1, tb1, SITE_ID(), __LINE__), ta, SITE_ID(), __LINE__);
	fp_entry* tt42 = T_SUB(T_SUB(ta1, tb1), ta);
	
	// fp_entry* td = tmpctx->t_add(tmpctx->t_const(1.0, SITE_ID(), __LINE__), tmpctx->t_mul(ta, ta, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* td = T_ADD(T_MUL(ta, ta), T_CONST(1.0));

	// fp_entry* ts = tmpctx->t_div(tt, td, SITE_ID(), __LINE__);
	fp_entry* ts = T_DIV(tt, td);

	// fp_entry* ts42 = tmpctx->t_div(tt42, td, SITE_ID(), __LINE__);
	fp_entry* ts42 = T_DIV(tt42, td);

	// return x1 + (((((((((2.0 * x1) * s) * (s - 3.0)) + ((x1 * x1) * ((4.0 * s) - 6.0))) * d) + (((3.0 * x1) * x1) * s)) + ((x1 * x1) * x1)) + x1) + (3.0 * s_42_));
	// fp_entry* tret1 = tmpctx->t_mul(tmpctx->t_mul(ta, tmpctx->t_const(2.0, SITE_ID(), __LINE__), SITE_ID(), __LINE__), ts, SITE_ID(), __LINE__);
	//((2.0 * x1) * s) * (s - 3.0))
	fp_entry* tret1 = T_MUL(T_MUL(T_MUL(ta, T_CONST(2.0)), ts), T_SUB(ts, T_CONST(3.0)));
	// ((x1 * x1) * ((4.0 * s) - 6.0))
	fp_entry* tret2 = T_MUL(T_MUL(ta, ta), T_SUB(T_MUL(T_CONST(4.0), ts), T_CONST(6.0)));
	fp_entry* tret3 = T_ADD(T_MUL(T_ADD(tret1, tret2), td), T_MUL(ta1, ts));
	// fp_entry* tret4 = T_ADD(T_MUL(ta1, ts), tret3);
	fp_entry* tresult = T_ADD(ta, T_ADD(T_ADD(T_ADD(tret3, T_MUL(T_MUL(ta, ta), ta)), ta), T_MUL(T_CONST(3.0), ts42)));
	cout << "org ex0 value: " << ex0(a, b) << endl;
	tmpctx->dump_tracing(tresult);
}