#include "comparison_function.h"

ComparisonFunction::ComparisonFunction(vector<ldouble> parameters){
    this->parameters = parameters;
}
ldouble ComparisonFunction::operator() (ldouble a, ldouble b){
    return 0;
}