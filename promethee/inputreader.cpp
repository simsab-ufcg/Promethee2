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

  string token;
  vector<string> input;

  while(in >> token) {
    input.push_back(token);
  }

  meta.weight = stod(input[0]);
  string funType = input[1];
  string parameters_line = input[2];
  meta.isMax = stoi(input[3]);
  meta.name = path;

  ldouble param;
  vector<ldouble> parameters;
  stringstream lineSplitter(parameters_line);
  
  while(lineSplitter >> param) {
    parameters.push_back(param);
  }

  // only linear by now
  meta.function = new PrometheeFunctionAdapter(is_opt, parameters, funType);
  return meta;
}