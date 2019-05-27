#include "comparison_function.h"

struct LevelComparator: ComparisonFunction{
    // parameters[0] = p
    // parameters[1] = q
    // both p,q should be positive numbers and p > q

    LevelComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator () (ldouble a, ldouble b);
    ldouble getP();

    ldouble getQ();
};