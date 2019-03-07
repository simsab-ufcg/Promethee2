#ifndef PROMETHEE_INCLUDED
  
  #define PROMETHEE_INCLUDED
  
  #include "../types.h"
  #include "../utils.h"
  #include "../data.h"
  #include "../promethee.h"
  #include "../functions/optimized/promethee_function.h"
  #include <algorithm>
  
  struct PrometheeOpt : Promethee {

    string filename; // name of criterion to be processed
    double weight; // weight of criterion
    uint16 sampleFormat; // size of bytes
    PrometheeFunction *function; // function to be used in processing
    int divideBy; // division constant , -1 to divide by (Area - 1)
    int chunkBound; // max chunk size
    bool isMax; // kind of function
    int area; // area of criterion

    // info related to criterion file
    int height, width, samplePerPixel;

    void process();
    void init(vector<string> args, int divideBy);
  };

#endif
