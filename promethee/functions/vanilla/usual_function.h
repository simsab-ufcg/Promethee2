#include "comparison_function.h"

struct UsualComparator: ComparisonFunction{
    UsualComparator(vector<ldouble> parameters): ComparisonFunction(parameters) {}
    ldouble operator()(ldouble a, ldouble b); 
};