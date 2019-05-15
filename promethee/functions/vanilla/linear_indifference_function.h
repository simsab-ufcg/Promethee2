#include "comparison_function.h"

struct LinearWithIndifferenceComparator: ComparisonFunction{
    // parameters[0] = p
    // parameters[1] = q
    // both p,q should be positive numbers and p > q

    LinearWithIndifferenceComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator() (ldouble a, ldouble b);
    ldouble getP();

    ldouble getQ();

};