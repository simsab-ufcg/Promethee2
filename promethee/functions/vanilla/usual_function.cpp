#include "usual_function.h"

ldouble UsualComparator::operator()(ldouble a, ldouble b){
    ldouble diff = a - b;
    if(diff <= 0)
        return 0;
    else
        return 1;
}