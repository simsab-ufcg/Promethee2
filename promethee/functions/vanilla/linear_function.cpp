#include "linear_function.h"

ldouble LinearComparator::getP(){
    return parameters[0];
}

ldouble LinearComparator::operator () (ldouble a, ldouble b){
    ldouble diff = a - b;
    if(diff < 0)
        return 0;
    else if(diff < getP())
        return diff / getP();
    else
        return 1;
}