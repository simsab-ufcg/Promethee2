#include "comparison_function.h"

struct LinearComparator: ComparisonFunction{
    // parameters[0] = p
    // p should be positive

    LinearComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator () (ldouble a, ldouble b);
    ldouble getP();
};