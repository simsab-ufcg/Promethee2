#ifndef PROMETHEE_INCLUDED
  
  #define PROMETHEE_INCLUDED
  
  #include "../types.h"
  #include "../data.h"
  #include "../promethee.h"
  #include <algorithm>
  
  struct PrometheeOpt : Promethee {

    string pathToOutput;
    vector<string> inputFiles, metaFiles;

    Data readData();
    void process();
    void init(vector<string> args, int divideBy);
  };

#endif
