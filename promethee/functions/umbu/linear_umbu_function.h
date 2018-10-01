#pragma once

#include "promethee_umbu_fun.h"

/**
 * Linear umbu function
 * */
struct LinearUmbuFunction : PrometheeUmbuFunction {

    LinearUmbuFunction(vector<ldouble> params): PrometheeUmbuFunction(params){}

    inline ldouble getPParameter();

    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;
    
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt) override;

};