#include "quasi_function.h"

ldouble QuasiComparator::getQ(){
    return parameters[0];
}

ldouble QuasiComparator::operator () (ldouble a, ldouble b){
    ldouble diff = a - b;
    if(diff <= getQ())
        return 0;
    else
        return 1;
}