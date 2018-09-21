#include "linear_indifference_function.h"

ldouble LinearWithIndifferenceComparator::getP(){
    return parameters[0];
}

ldouble LinearWithIndifferenceComparator::getQ(){
    return parameters[1];
}

ldouble LinearWithIndifferenceComparator::operator () (ldouble a, ldouble b) {
    ldouble diff = a - b;
    if(diff <= getQ())
        return 0;
    else if(diff <= getP())
        return (diff - getQ())/(getP() - getQ());
    else
        return 1;
}