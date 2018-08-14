#ifndef OUTPUT_WRITER_INCLUDED
  
  #define OUTPUT_WRITER_INCLUDED
  
  #include "types.h"
  #include <fstream>
  #include <iomanip>
  
  struct OutputWriter{
    void write(string path, PrometheeResult result);
  };

#endif
