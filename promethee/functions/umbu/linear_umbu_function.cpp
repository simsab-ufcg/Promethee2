#include "linear_umbu_function.h"
#include <algorithm>

ldouble LinearUmbuFunction::getPParameter(){
    return this->params[0];
}

ldouble LinearUmbuFunction::getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;
    
    /**
     * Keep the p parameter to have faster access
     * */
    ldouble pParameter = this->getPParameter();
    
    {
        /**
         * Count the numbers in interval [-Infinity, QueryValue - pParameter)
         * This QueryValue will be completely prefered than this numbers, 
         * each one will contribute with 1 in the positiveDelta
         * 
         * ptr2 = pointer the first number that is not in the interval
         * 
         * cnt[ptr2 - 1] = amount of numbers in the interval
         * 
         * As every contributes with 1, still have to multiply be the weight
         * */
        int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
        ldouble value = weight * (ptr2 > 0 ? cnt[ptr2 - 1] : 0);
        sum += value;
    
        /**
         * Count the numbers in interval [QueryValue - pParameter, QueryValue)
         * Query value will be preferred in a linear function way,
         * the contribution of each one is (QueryValue - x) / pParameter,
         * the summation of every alternative in this interval will result in
         * (QueryValue * (numbers in terminal) - (summation of alternatives))/pParameter
         * 
         * ptr = pointer the first number that is not in the interval [- Infinity, QueryValue)
         * 
         * cnt[ptr - 1] = amount of numbers in the interval [-Infinity, QueryValue)
         * 
         * cnt[ptr - 1] - cnt[ptr2 - 1] = amount of numbers in the interval [QueryValue - pParameter, QueryValue)
         * 
         * The same logic is applied to cummulative to get the summation of every alternative in the interval
         * 
         * The cases where ptr or ptr2 have to handled, cnt[-1] = 0
         * */
        int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
        unsigned int amount = 0;
        if(ptr > 0) amount += cnt[ptr - 1];
        if(ptr2 > 0) amount -= cnt[ptr2 - 1];
        if (amount > 0){
            ldouble value = 0;
            if (ptr > 0)
                value += cummulative[ptr - 1];
            if (ptr2 > 0)
                value -= cummulative[ptr2 - 1];
            ldouble fvalue = weight * (amount * queryValue - value) / pParameter;
            sum += fvalue;
        }
    }
    return sum;
}

ldouble LinearUmbuFunction::getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;
    
    ldouble pParameter = this->getPParameter();
    
    {
        int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
        if (ptr2 < values.size()){
            ldouble value = weight * (cnt.back() - (ptr2 > 0 ? cnt[ptr2 - 1] : 0));
            sum += value;
        }

        int ptr = upper_bound(values.begin(), values.end(), queryValue) - values.begin();
        unsigned int amount = 0;
        if(ptr2 > 0) amount += cnt[ptr2 - 1];
        if(ptr > 0) amount -= cnt[ptr - 1];
        if (amount > 0){
            ldouble value = 0;
            if (ptr2 > 0)
                value += cummulative[ptr2 - 1];
            if (ptr > 0)
                value -= cummulative[ptr - 1];
            ldouble fvalue = weight * (value - amount * queryValue) / pParameter;
            sum += fvalue;
        }
    }
    return sum;
}