#ifndef MATRIX_META_DATA_INCLUDED
    
    #define MATRIX_META_DATA_INCLUDED    
    
    #include "types.h"
    #include "promethee_function.h"
    #include <string>
    #include <iostream>
    
    struct MatrixMetaData{
        PrometheeFunction* function;
        bool isMax;
        string name;
        double weight;
    };

#endif
