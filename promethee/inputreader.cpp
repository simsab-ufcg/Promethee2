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
  ldouble pParameter;
  if(in >> funType){
    in >> pParameter;
    in >> meta.isMax;
  } else {
    meta.isMax = true;
    pParameter = 1.0;
  }
  meta.name = path;
  // only linear by now
  meta.function = new LinearFunction(pParameter);
  return meta;
}
