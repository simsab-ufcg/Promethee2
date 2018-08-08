#ifndef INPUTREADER_INCLUDED
  #include "types.h"
  #include <fstream>
  #include <sstream>
  #include "matrix_meta_data.h"
  struct InputReader{
    Matrix readMatrix(string path);
    MatrixMetaData readMetaData(string path);
  };
#endif
#define INPUTREADER_INCLUDED
