#include "level_function.h"

ldouble LevelComparator::getP(){
    return parameters[0];
}

ldouble LevelComparator::getQ(){
    return parameters[1];
}

ldouble LevelComparator::operator () (ldouble a, ldouble b){
    ldouble diff = a - b;
    if(diff <= getQ())
        return 0;
    else if(diff <= getP())
        return 0.5;
    else
        return 1;
}