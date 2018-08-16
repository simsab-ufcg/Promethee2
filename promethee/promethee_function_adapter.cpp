#include "promethee_function_adapter.h"

ldouble PrometheeFunctionAdapter::getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight){
    return (*optFunction).getPositiveDelta(values, queryValue, cummulative, weight);
}

ldouble PrometheeFunctionAdapter::getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight){
    return (*optFunction).getNegativeDelta(values, queryValue, cummulative, weight);
}