#include <iostream>
#include <eft/eft.hpp>
using namespace std;
using namespace eft;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
#define TRUE 1
#define FALSE 0

// double ex0(double a, double b) {
// 	return (((333.75 * pow(b, 6.0)) + (pow(a, 2.0) * (((((11.0 * pow(a, 2.0)) * pow(b, 2.0)) - pow(b, 6.0)) - (121.0 * pow(b, 4.0))) - 2.0))) + (5.5 * pow(b, 8.0))) + (a / (2.0 * b));
// }

double ex1(double a, double b) {
	double b2 = b * b;
	double b4 = b2 * b2;
	double b6 = b4 * b2;
	double b8 = b4 * b4;
	double a2 = a * a;
	double firstexpr = ((((11.0 * a2) * b2) - b6) - (121.0 * b4)) - 2.0;
	return (((333.75 * b6) + (a2 * firstexpr)) + (5.5 * b8)) + (a / (2.0 * b));
}

double ex2(double a, double b) {
	double b2 = b * b;
	double b4 = b2 * b2;
	double b6 = b4 * b2;
	double b8 = b4 * b4;
	double a2 = a * a;
	double firstexpr = (((11.0 * a2) * b2) - (121.0 * b4)) - 2.0;
	return ((((333.75 - a2) * b6) + (a2 * firstexpr)) + (5.5 * b8)) + (a / (2.0 * b));
}

int main() {
	// ex0(1, 2);
	double a = 2;
	double b = 3;
	tmpctx->t_store(&a, tmpctx->t_const(a, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	tmpctx->t_store(&b, tmpctx->t_const(b, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* ta = tmpctx->t_load(&a, SITE_ID(), __LINE__);
	fp_entry* tb = tmpctx->t_load(&b, SITE_ID(), __LINE__);
	fp_entry* ta2 = tmpctx->t_mul(ta, ta, SITE_ID(), __LINE__);
	fp_entry* tb2 = tmpctx->t_mul(tb, tb, SITE_ID(), __LINE__);
	fp_entry* tb4 = tmpctx->t_mul(tb2, tb2, SITE_ID(), __LINE__);
	fp_entry* tb6 = tmpctx->t_mul(tb2, tb4, SITE_ID(), __LINE__);
	fp_entry* tb8 = tmpctx->t_mul(tb4, tb4, SITE_ID(), __LINE__);
	fp_entry* tfirstexpr = tmpctx->t_mul(tmpctx->t_const(11.0, SITE_ID(), __LINE__), ta2, SITE_ID(), __LINE__);
	tfirstexpr = tmpctx->t_mul(tfirstexpr, tb2, SITE_ID(), __LINE__);
	tfirstexpr = tmpctx->t_sub(tfirstexpr, tb6, SITE_ID(), __LINE__);
	tfirstexpr = tmpctx->t_sub(tfirstexpr, tmpctx->t_mul(tmpctx->t_const(121.0, SITE_ID(), __LINE__), tb4, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	tfirstexpr = tmpctx->t_sub(tfirstexpr, tmpctx->t_const(2.0, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* tret1 = tmpctx->t_mul(tmpctx->t_const(333.75, SITE_ID(), __LINE__), tb6, SITE_ID(), __LINE__);
	fp_entry* tret2 = tmpctx->t_mul(ta2, tfirstexpr, SITE_ID(), __LINE__);
	fp_entry* tret3 = tmpctx->t_mul(tmpctx->t_const(5.5, SITE_ID(), __LINE__), tb8, SITE_ID(), __LINE__);
	fp_entry* tret4 = tmpctx->t_div(ta, tmpctx->t_mul(tmpctx->t_const(2.0, SITE_ID(), __LINE__), tb, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* tresult = tmpctx->t_add(tmpctx->t_add(tret1, tret2, SITE_ID(), __LINE__), tmpctx->t_add(tret3, tret4, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	tmpctx->dump_tracing(tresult);
	cout << "org ex1 value: " << ex1(a, b) << endl;
	
	double a1 = 3;
	double b1 = 4;
	tmpctx->t_store(&a1, tmpctx->t_const(a1, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	tmpctx->t_store(&b1, tmpctx->t_const(b1, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* tta = tmpctx->t_load(&a1, SITE_ID(), __LINE__);
	fp_entry* ttb = tmpctx->t_load(&b1, SITE_ID(), __LINE__);
	fp_entry* tta2 = tmpctx->t_mul(tta, tta, SITE_ID(), __LINE__);
	fp_entry* ttb2 = tmpctx->t_mul(ttb, ttb, SITE_ID(), __LINE__);
	fp_entry* ttb4 = tmpctx->t_mul(ttb2, ttb2, SITE_ID(), __LINE__);
	fp_entry* ttb6 = tmpctx->t_mul(ttb2, ttb4, SITE_ID(), __LINE__);
	fp_entry* ttb8 = tmpctx->t_mul(ttb4, ttb4, SITE_ID(), __LINE__);
	fp_entry* ttfirstexpr = tmpctx->t_mul(tmpctx->t_const(11.0, SITE_ID(), __LINE__), tta2, SITE_ID(), __LINE__);
	ttfirstexpr = tmpctx->t_mul(ttfirstexpr, ttb2, SITE_ID(), __LINE__);
	ttfirstexpr = tmpctx->t_sub(ttfirstexpr, ttb6, SITE_ID(), __LINE__);
	ttfirstexpr = tmpctx->t_sub(ttfirstexpr, tmpctx->t_mul(tmpctx->t_const(121.0, SITE_ID(), __LINE__), ttb4, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	ttfirstexpr = tmpctx->t_sub(ttfirstexpr, tmpctx->t_const(2.0, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* ttret1 = tmpctx->t_mul(tmpctx->t_sub(tmpctx->t_const(333.75, SITE_ID(), __LINE__), tta2, SITE_ID(), __LINE__), ttb6, SITE_ID(), __LINE__);
	fp_entry* ttret2 = tmpctx->t_mul(tta2, ttfirstexpr, SITE_ID(), __LINE__);
	fp_entry* ttret3 = tmpctx->t_mul(tmpctx->t_const(5.5, SITE_ID(), __LINE__), ttb8, SITE_ID(), __LINE__);
	fp_entry* ttret4 = tmpctx->t_div(tta, tmpctx->t_mul(tmpctx->t_const(2.0, SITE_ID(), __LINE__), ttb, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	fp_entry* ttresult = tmpctx->t_add(tmpctx->t_add(ttret1, ttret2, SITE_ID(), __LINE__), tmpctx->t_add(ttret3, ttret4, SITE_ID(), __LINE__), SITE_ID(), __LINE__);
	tmpctx->dump_tracing(ttresult);
	cout << "org ex1 value: " << ex2(a, b) << endl;
}