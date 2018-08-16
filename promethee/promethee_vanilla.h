#ifndef PROMETHEE_VANILLA_INCLUDED
  
  #define PROMETHEE_VANILLA_INCLUDED
  
  #include "types.h"
  #include "data.h"
  #include "promethee_function_adapter.h"
  #include "normalize.h"
  #include <iostream>
  #include <algorithm>
  
  struct PrometheeVanilla {
    PrometheeResult process(Data data);
  };

#endif
