#ifndef LINEAR_FUNCTION_INCLUDED
    
    #define LINEAR_FUNCTION_INCLUDED
    
    #include "promethee_function.h"
    
    struct LinearFunction : PrometheeFunction {
        ldouble pParameter;
        
        LinearFunction(ldouble pParameter): PrometheeFunction(){
            this->pParameter = pParameter;
        }
        
        ldouble getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;
        ldouble getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;
        
        ldouble getPParameter(){
            return pParameter;
        }
    
    };
#endif