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
    
    ldouble pParameter = this->getPParameter();
    ldouble qParameter = this->getQParameter();

    {
        int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
        ldouble value = weight * (ptr2 > 0 ? cnt[ptr2 - 1] : 0);
        sum += value;

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
        if(ptr > 0) amount += cnt[ptr - 1];
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