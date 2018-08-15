#include "comparison_function.h"

struct GaussianComparator: ComparisonFunction{
    //parameters[0] = sigma

    GaussianComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator () (ldouble a, ldouble b);
    ldouble getSigma();
};