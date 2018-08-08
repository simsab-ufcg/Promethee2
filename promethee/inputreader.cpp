#include "inputreader.h"
#include "linear_function.h"
#include <iostream>
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
MatrixMetaData InputReader::readMetaData(string path){
  ifstream in(path);
  MatrixMetaData meta;
  string funType;
  in >> funType;
  ldouble pParameter;
  in >> pParameter;
  // only linear by now
  meta.function = new LinearFunction(pParameter);
  in >> meta.isMax;
  meta.name = path;
  return meta;
}
