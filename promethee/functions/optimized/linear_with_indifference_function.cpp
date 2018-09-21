#include "linear_with_indifference_function.h"
#include <iostream>

ldouble LinearWithIndifferenceFunction::getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight) {
    ldouble sum = 0;

    {
        int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
        ldouble value = weight * ptr2;
        sum += value;

        int ptr = lower_bound(values.begin(), values.end(), queryValue - qParameter) - values.begin();
        int amount = ptr - ptr2;
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

ldouble LinearWithIndifferenceFunction::getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight) {
    ldouble sum = 0;

    {
        int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
        if(ptr2 < values.size()){
            ldouble value = weight * (values.size() - ptr2);
            sum += value;
        }

        int ptr = upper_bound(values.begin(), values.end(), queryValue + qParameter) - values.begin();
        int amount = ptr2 - ptr;
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