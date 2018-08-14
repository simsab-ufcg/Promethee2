#ifndef NORMALIZE_INCLUDED
  
  #define NORMALIZE_INCLUDED
  
  #include "types.h"
  
  struct Normalizer {
    Matrix normalize(Matrix matrix, MaskMatrix validPixels);
  };

#endif