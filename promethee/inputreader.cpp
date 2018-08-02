#include "inputreader.h"
Matrix InputReader::readMatrix(string path){
  ifstream in(path);
  Matrix matrix;
  string line;
  while(getline(in, line)){
    stringstream lineReader(line);
    ldouble value;
    MatrixLine nline;
    while(lineReader >> value)
      nline.push_back(value);
    matrix.push_back(nline);
  }
  return matrix;
}
