#ifndef PROMETHEE_VANILLA_INCLUDED
  
  #define PROMETHEE_VANILLA_INCLUDED
  
  #include "../types.h"
  #include "../data.h"
  #include "../promethee_function_adapter.h"
  #include "../promethee.h"
  #include "../normalize.h"
  #include <iostream>
  #include <algorithm>
  
  struct PrometheeVanilla : Promethee {
    Data readData();
    void process();
  };

#endif
