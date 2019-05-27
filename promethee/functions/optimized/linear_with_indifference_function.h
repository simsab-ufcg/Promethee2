#ifndef LINEAR_WITH_INDIFFERENCE

    #define LINEAR_WITH_INDIFFERENCE

    #include "promethee_function.h"

    struct LinearWithIndifferenceFunction : PrometheeFunction {
        
        ldouble pParameter, qParameter;

        LinearWithIndifferenceFunction(vector<ldouble> parameters) : PrometheeFunction() {
            this->pParameter = parameters[0];
            this->qParameter = parameters[1];
        }

        ldouble getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;
        ldouble getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;

    };

#endif