#ifndef COMPARISON_FUNCTION_INCLUDED

    #define COMPARISON_FUNCTION_INCLUDED

    #include "../types.h"

    struct ComparisonFunction{
        vector<ldouble> parameters;
        ComparisonFunction(vector<ldouble> parameters);
        virtual ldouble operator () (ldouble a, ldouble b);
    };

#endif