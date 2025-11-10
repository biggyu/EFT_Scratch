#include <iostream>
#include <eft/eft.hpp>
using namespace std;
using namespace eft;
ShadowMemory* smem = new ShadowMemory();
TempContext* tmpctx = new TempContext(smem, 256);
#define TRUE 1
#define FALSE 0

double ex0(double a, double b, double c, double d, double e, double f, double g, double h, double i) {
	return ((((a * e) * i) + ((b * f) * g)) + ((c * d) * h)) - ((((c * e) * g) + ((b * d) * i)) + ((a * f) * h));
}

int main() {
	double a = 1;
	double b = 2;
	double c = 3;
	double d = 4;
	double e = 5;
	double f = 6;
	double g = 7;
	double h = 8;
	double i = 9;
	T_STORE(&a, T_CONST(a));
	T_STORE(&b, T_CONST(b));
	T_STORE(&c, T_CONST(c));
	T_STORE(&d, T_CONST(d));
	T_STORE(&e, T_CONST(e));
	T_STORE(&f, T_CONST(f));
	T_STORE(&g, T_CONST(g));
	T_STORE(&h, T_CONST(h));
	T_STORE(&i, T_CONST(i));
	fp_entry* ta = T_LOAD(&a);
	fp_entry* tb = T_LOAD(&b);
	fp_entry* tc = T_LOAD(&c);
	fp_entry* td = T_LOAD(&d);
	fp_entry* te = T_LOAD(&e);
	fp_entry* tf = T_LOAD(&f);
	fp_entry* tg = T_LOAD(&g);
	fp_entry* th = T_LOAD(&h);
	fp_entry* ti = T_LOAD(&i);
	// return ((((a * e) * i) + ((b * f) * g)) + ((c * d) * h)) - ((((c * e) * g) + ((b * d) * i)) + ((a * f) * h));
	// fp_entry* tret1 = T_MUL(T_MUL(ta, te), ti);
	// fp_entry* tret2 = T_MUL(T_MUL(tb, tf), tg);
	// fp_entry* tret3 = T_MUL(T_MUL(tc, td), th);
	// fp_entry* treturn1 = T_ADD(T_ADD(tret1, tret2), tret3);
	fp_entry* treturn1 = T_ADD(T_ADD(T_MUL(T_MUL(ta, te), ti), T_MUL(T_MUL(tb, tf), tg)), T_MUL(T_MUL(tc, td), th));
	// fp_entry* tret4 = T_MUL(T_MUL(tc, te), tg);
	// fp_entry* tret5 = T_MUL(T_MUL(tb, td), ti);
	// fp_entry* tret6 = T_MUL(T_MUL(ta, tf), th);
	// fp_entry* treturn2 = T_ADD(T_ADD(tret4, tret5), tret6);
	// tmpctx->dump_tracing(tret6);
	fp_entry* treturn2 = T_ADD(T_ADD(T_MUL(T_MUL(tc, te), tg), T_MUL(T_MUL(tb, td), ti)), T_MUL(T_MUL(ta, tf), th));
	tmpctx->dump_tracing(treturn2);
	fp_entry* tresult = T_SUB(treturn1, treturn2);
	
	cout << "org ex0 value: " << ex0(1, 2, 3, 4, 5, 6, 7, 8, 9) << endl;
	tmpctx->dump_tracing(tresult);
}