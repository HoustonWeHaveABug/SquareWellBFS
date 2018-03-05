#include <stdio.h>
#include "fractions.h"

int lcm(int, int);
int gcd(int, int);

void divide_fraction_int(fraction_t *a, int b, fraction_t *result) {
	fraction_t b_inverse;
	set_fraction(&b_inverse, 1, b, 0);
	multiply_fractions(a, &b_inverse, result);
}

void add_fraction_int(fraction_t *a, int b, fraction_t *result) {
	set_fraction(result, a->numerator+b*a->denominator, a->denominator, 0);
}

void add_fractions(fraction_t *a, fraction_t *b, fraction_t *result) {
	int denominators_lcm = lcm(a->denominator, b->denominator);
	set_fraction(result, a->numerator*denominators_lcm/a->denominator+b->numerator*denominators_lcm/b->denominator, denominators_lcm, 1);
}

void multiply_fraction_int(fraction_t *a, int b, fraction_t *result) {
	set_fraction(result, a->numerator*b, a->denominator, 1);
}

void multiply_fractions(fraction_t *a, fraction_t *b, fraction_t *result) {
	set_fraction(result, a->numerator*b->numerator, a->denominator*b->denominator, 1);
}

void subtract_fractions(fraction_t *a, fraction_t *b, fraction_t *result) {
	int denominators_lcm = lcm(a->denominator, b->denominator);
	set_fraction(result, a->numerator*denominators_lcm/a->denominator-b->numerator*denominators_lcm/b->denominator, denominators_lcm, 1);
}

void set_fraction(fraction_t *fraction, int numerator, int denominator, int reduce) {
	if (reduce) {
		int fraction_gcd = gcd(numerator, denominator);
		numerator /= fraction_gcd;
		denominator /= fraction_gcd;
	}
	fraction->numerator = numerator;
	fraction->denominator = denominator;
}

int compare_fractions(fraction_t *a, fraction_t *b) {
	int denominators_lcm = lcm(a->denominator, b->denominator);
	return a->numerator*denominators_lcm/a->denominator-b->numerator*denominators_lcm/b->denominator;
}

void print_fraction(fraction_t *fraction) {
	printf("%d", fraction->numerator);
	if (fraction->numerator != 0 && fraction->denominator > 1) {
		printf("/%d", fraction->denominator);
	}
}

int lcm(int a, int b) {
	if (a < b) {
		return a*b/gcd(b, a);
	}
	return a*b/gcd(a, b);
}

int gcd(int a, int b) {
	int m = a%b;
	if (m > 0) {
		return gcd(b, m);
	}
	return b;
}
