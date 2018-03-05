typedef struct {
	int numerator;
	int denominator;
}
fraction_t;

void divide_fraction_int(fraction_t *, int, fraction_t *);
void add_fraction_int(fraction_t *, int, fraction_t *);
void add_fractions(fraction_t *, fraction_t *, fraction_t *);
void multiply_fraction_int(fraction_t *, int, fraction_t *);
void multiply_fractions(fraction_t *, fraction_t *, fraction_t *);
void subtract_fractions(fraction_t *, fraction_t *, fraction_t *);
void set_fraction(fraction_t *, int, int, int);
int compare_fractions(fraction_t *, fraction_t *);
void print_fraction(fraction_t *);
