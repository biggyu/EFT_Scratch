#include <iostream>
#include <eft/eft.hpp>
using namespace std;
using namespace eft;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
#define TRUE 1
#define FALSE 0

double ex0(double x1, double x2, double x3, double x4) {
	return (((((((x1 * x4) * (((-x1 + x2) + x3) - x4)) + (x2 * (((x1 - x2) + x3) + x4))) + (x3 * (((x1 + x2) - x3) + x4))) - ((x2 * x3) * x4)) - (x1 * x3)) - (x1 * x2)) - x4;
}

int main() {
	double a = 1;
	double b = 2;
	double c = 3;
	double d = 4;
	T_STORE(&a, T_CONST(a));
	T_STORE(&b, T_CONST(b));
	T_STORE(&c, T_CONST(c));
	T_STORE(&d, T_CONST(d));
	fp_entry* ta = T_LOAD(&a);
	fp_entry* tb = T_LOAD(&b);
	fp_entry* tc = T_LOAD(&c);
	fp_entry* td = T_LOAD(&d);
	fp_entry* tret1 = T_MUL(T_MUL(ta, td), T_SUB(T_ADD(T_SUB(tb, ta), tc), td));
	fp_entry* tret2 = T_ADD(tret1, T_MUL(tb, T_ADD(T_ADD(T_SUB(ta, tb), tc), td)));
	fp_entry* tret3 = T_SUB(T_SUB(T_ADD(tret2, T_MUL(tc, T_ADD(T_SUB(T_ADD(ta, tb), tc), td))), T_MUL(T_MUL(tb, tc), td)), T_MUL(ta, tc));
	fp_entry* tret4 = T_SUB(T_SUB(tret3, T_MUL(ta, tb)), td);
	cout << "org ex1 value: " << ex0(1, 2, 3, 4) << endl;
	tmpctx->dump_tracing(tret4);
}