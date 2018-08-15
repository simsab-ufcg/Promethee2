#include "comparison_function.h"

struct QuasiComparator: ComparisonFunction{
    // parameters[0] = q
    // q should be positive

    QuasiComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator () (ldouble a, ldouble b);
    ldouble getQ();
};