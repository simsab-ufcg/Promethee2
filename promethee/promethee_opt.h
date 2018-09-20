#ifndef PROMETHEE_INCLUDED
  
  #define PROMETHEE_INCLUDED
  
  #include "types.h"
  #include "data.h"
  #include "promethee.h"
  #include <algorithm>
  
  struct PrometheeOpt : Promethee {
    Data readData();
    void process();
  };

#endif
