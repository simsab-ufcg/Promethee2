#ifndef PROMETHEE_FUN
    #include <vector>
    #include "types.h"
    struct PrometheeFunction {
        ldouble getPositiveFlow(vector<ldouble> & values, ldouble queryValue);
        ldouble getNegativeFlow(vector<ldouble> & values, ldouble queryValue);
    };
#endif
#define PROMETHEE_FUN
