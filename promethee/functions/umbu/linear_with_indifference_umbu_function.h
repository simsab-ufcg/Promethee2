#pragma once

#include "promethee_umbu_fun.h"

struct LinearWithIndifferenceUmbuFunction : PrometheeUmbuFunction {

    LinearWithIndifferenceUmbuFunction(vector<ldouble> params): PrometheeUmbuFunction(params){}

    inline ldouble getPParameter();
    
    inline ldouble getQParameter();

    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;
    
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;

};