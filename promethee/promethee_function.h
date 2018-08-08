#ifndef PROMETHEE_FUN
    #include <vector>
    #include "types.h"
    #include <algorithm>
    using namespace std;
    struct PrometheeFunction {
        virtual ldouble getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight);
        virtual ldouble getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight);
    };
#endif
#define PROMETHEE_FUN
