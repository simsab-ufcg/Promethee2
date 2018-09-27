#include "linear_function.h"
#include <algorithm>

ldouble LinearFunction::getPParameter(){
    return this->params[0];
}

ldouble LinearFunction::getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;
    {
        int ptr2 = lower_bound(values.begin(), values.end(), queryValue - this->getPParameter()) - values.begin();
        ldouble value = weight * (ptr2 > 0 ? cnt[ptr2 - 1] : 0);
        sum += value;
    
        int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
        int amount = 0;
        if(ptr > 0) amount += cnt[ptr - 1];
        if(ptr2 > 0) amount -= cnt[ptr2 - 1];
        if (amount > 0){
            ldouble value = 0;
            if (ptr > 0)
                value += cummulative[ptr - 1];
            if (ptr2 > 0)
                value -= cummulative[ptr2 - 1];
            ldouble fvalue = weight * (amount * queryValue - value) / this->getPParameter();
            sum += fvalue;
        }
    }
    return sum;
}

ldouble LinearFunction::getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
    ldouble sum = 0;
    {
        int ptr2 = upper_bound(values.begin(), values.end(), queryValue + this->getPParameter()) - values.begin();
        if (ptr2 < values.size()){
            ldouble value = weight * (cnt.back() - (ptr2 > 0 ? cnt[ptr2 - 1] : 0));
            sum += value;
        }

        int ptr = upper_bound(values.begin(), values.end(), queryValue) - values.begin();
        int amount = 0;
        if(ptr > 0) amount += cnt[ptr - 1];
        if(ptr2 > 0) amount -= cnt[ptr2 - 1];
        if (amount > 0){
            ldouble value = 0;
            if (ptr2 > 0)
                value += cummulative[ptr2 - 1];
            if (ptr > 0)
                value -= cummulative[ptr - 1];
            ldouble fvalue = weight * (value - amount * queryValue) / this->getPParameter();
            sum += fvalue;
        }
    }
    return sum;
}