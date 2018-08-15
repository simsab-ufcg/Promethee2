#include "inputreader.h"
#include "linear_function.h"
#include <iostream>
#include <limits>
#include <cmath>

Matrix InputReader::readMatrix(string path){
  ifstream in(path);
  Matrix matrix;
  string line;
  while(getline(in, line)){
    stringstream lineReader(line);
    string token;
    ldouble value;
    MatrixLine nline;
    while(lineReader >> token){
      if(token == "nan")
        value = NAN;
      else
        value = atof(token.c_str());
      nline.push_back(value);
    }
    matrix.push_back(nline);
  }
  return matrix;
}
MatrixMetaData InputReader::readMetaData(string path, bool is_opt){
  ifstream in(path);
  MatrixMetaData meta;
  string funType;
  string parameters_line;
  in >> meta.weight;
  in >> funType;
  in >> parameters_line;
  vector<ldouble> parameters;
  ldouble param;
  stringstream lineSplitter(parameters_line);
  while(lineSplitter >> param)
    parameters.push_back(param);
  in >> meta.isMax;
  meta.name = path;
  // only linear by now
  meta.function = new PrometheeFunctionAdapter(is_opt, parameters, funType);
  return meta;
}