#include "linear_with_indifference_umbu_function.h"
#include <algorithm>

ldouble LinearWithIndifferenceUmbuFunction::getPParameter(){
    return params[0];
}

ldouble LinearWithIndifferenceUmbuFunction::getQParameter(){
    return params[1];
}

ldouble LinearWithIndifferenceUmbuFunction::getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;
    
    // Keep the p,q parameter to have faster access
    ldouble pParameter = this->getPParameter();
    ldouble qParameter = this->getQParameter();

    {
        //   Count the number of alternatives in interval [-Infinity, QueryValue - pParameter)
        //   QueryValue will be completely prefered than this alternatives, 
        //   each one will contribute with 1 in the positiveDelta
        //   ptr2 = pointer the first alternative that is not in the interval
        //   cnt[ptr2 - 1] = amount of alternatives in the interval
        //   As every contributes with 1, still have to multiply be the weight
        
        int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
        ldouble value = weight * (ptr2 > 0 ? cnt[ptr2 - 1] : 0);
        sum += value;

        //   Count the number of alternatives in interval [QueryValue - pParameter, QueryValue - qParameter)
        //   QueryValue will be preferred in a linear function way,
        //   the contribution of each one is (QueryValue - x) / (pParameter - qParameter),
        //   the summation of every alternative in this interval will result in
        //   (QueryValue * (number of alternatives in terminal) - (summation of alternatives))/(pParameter - qParameter)
        //   ptr = pointer the first alternative that is not in the interval [- Infinity, QueryValue - qParameter)
        //   cnt[ptr - 1] = amount of alternatives in the interval [-Infinity, QueryValue - qParameter)
        //   cnt[ptr - 1] - cnt[ptr2 - 1] = amount of alternatives in the interval [QueryValue - pParameter, QueryValue - qParameter)
        //   The same logic is applied to cummulative to get the summation of every alternative in the interval
        //   The cases where ptr or ptr2 have to handled, cnt[-1] = 0 
        
        int ptr = lower_bound(values.begin(), values.end(), queryValue - qParameter) - values.begin();
        unsigned int amount = 0;
        if(ptr > 0) amount += cnt[ptr - 1];
        if(ptr2 > 0) amount -= cnt[ptr2 - 1];
        if(amount > 0){
            ldouble value = 0;
            if(ptr > 0) value += cummulative[ptr - 1];
            if(ptr2 > 0) value -= cummulative[ptr2 - 1];
            ldouble fvalue = weight * ((amount * queryValue - value) - amount * qParameter) / (pParameter - qParameter);
            sum += fvalue;
        }
    }

    return sum;
}

ldouble LinearWithIndifferenceUmbuFunction::getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;

    ldouble pParameter = this->getPParameter();
    ldouble qParameter = this->getQParameter();

    {
        int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
        if(ptr2 < values.size()){
            ldouble value = weight * (cnt.back() - (ptr2 > 0 ? cnt[ptr2 - 1] : 0));
            sum += value;
        }

        int ptr = upper_bound(values.begin(), values.end(), queryValue + qParameter) - values.begin();
        unsigned int amount = 0;
        if(ptr2 > 0) amount += cnt[ptr2 - 1];
        if(ptr > 0) amount -= cnt[ptr - 1];
        if(amount > 0){
            ldouble value = 0;
            if(ptr2 > 0) value += cummulative[ptr2 - 1];
            if(ptr > 0) value -= cummulative[ptr - 1];
            ldouble fvalue = weight * ((value - amount * queryValue) - amount * qParameter)  / (pParameter - qParameter);
            sum += fvalue;        
        }
    }

    return sum;
}