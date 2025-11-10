#include <fenv.h>
#include <math.h>
#include <stdint.h>
#define TRUE 1
#define FALSE 0

double ex0(double a, double b, double c, double d, double e, double f, double g, double h, double i) {
	return ((((a * e) * i) + ((b * f) * g)) + ((c * d) * h)) - ((((c * e) * g) + ((b * d) * i)) + ((a * f) * h));
}

int main() {
	ex0(1, 2, 3, 4, 5, 6, 7, 8, 9);
}