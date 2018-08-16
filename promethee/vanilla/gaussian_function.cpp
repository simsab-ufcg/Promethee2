#include "gaussian_function.h"
#include <math.h>

ldouble GaussianComparator::getSigma(){
    return parameters[0];
}

ldouble GaussianComparator::operator () (ldouble a, ldouble b){
    ldouble diff = a - b;
    if(diff < 0)
        return 0;
    else
        return 1 - exp(- diff*diff / (2 * getSigma() * getSigma() ) );

}