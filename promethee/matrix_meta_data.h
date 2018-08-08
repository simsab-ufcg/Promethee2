#ifndef MATRIX_META_DATA_INCLUDED
    #include "types.h"
    #include "promethee_function.h"
    #include <string>
    #include <iostream>
    struct MatrixMetaData{
        PrometheeFunction* function;
        ldouble pParameter;
        bool isMax;
        string name;
        void show(){
            cerr << name << " " << isMax << " " << pParameter << endl;
        }
    };
#endif
#define MATRIX_META_DATA_INCLUDED