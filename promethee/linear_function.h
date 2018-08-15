#ifndef LINEAR_FUNCTION_INCLUDED
    
    #define LINEAR_FUNCTION_INCLUDED
    
    #include "promethee_function.h"
    
    struct LinearFunction : PrometheeFunction {
        ldouble pParameter;
        
        LinearFunction(vector<ldouble> pParameter): PrometheeFunction(){
            this->pParameter = pParameter[0];
        }
        
        ldouble getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;
        ldouble getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight) override;
        
        ldouble getPParameter(){
            return pParameter;
        }
    
    };
#endif