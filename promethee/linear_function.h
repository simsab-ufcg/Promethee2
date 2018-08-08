#include "promethee_function.h"

struct LinearFunction : PrometheeFunction {
    ldouble getPositiveFlow(vector<ldouble> & values, ldouble queryValue);
    ldouble getNegativeFlow(vector<ldouble> & values, ldouble queryValue);
};