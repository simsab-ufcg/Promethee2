#pragma once

#include "promethee_umbu_fun.h"

struct LinearFunction : PrometheeUmbuFunction {

    ldouble getPParameter();

    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;
    
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;

};