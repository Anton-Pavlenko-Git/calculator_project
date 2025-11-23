#include "math_lib.h"
#include <cmath> // for std::nan

double add(double a, double b) {
	return a+b;
}

double subtract(double a, double b) {
	return a-b;
}

double multiply(double a, double b) {
	return a*b;
}

double divide(double a, double b) {
	if (b==0.0) { return std::nan(""); }
	return a/b;
}

double power(double base, int exponent) {
	if(exponent == 0) { return 1.0; }
	bool is_negative_exponent = false;
	if(exponent < 0) {
		is_negative_exponent = true;
		exponent = -exponent;
	}
	double result = 1.0;
	for (int i=0; i<exponent; ++i) {
		result *= base;
	}
	if(is_negative_exponent) { return 1.0/result; }
	return result;
}

long long factorial (int n) {
	if(n<0) { return -1; }
	if(n==0 || n==1) {return 1; }

	return n*factorial(n-1);
}
