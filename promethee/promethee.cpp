#include "promethee.h"
#include "normalize.h"
#include "promethee_function_adapter.h"
#include <iostream>

PrometheeResult Promethee::process(Data data){
  int ncriterias = data.matrices.size();
  int nlines = data.matrices[0].size();
  int ncolumns = data.matrices[0][0].size();

  // getting matrix with information about valid pixels
  MaskMatrix validPixels = data.getMaskMatrix();

  Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  
  // applying the optimized promethee method (using linear preference function only)
  for(int criteria = 0; criteria < ncriterias; criteria++){

    Matrix matrix = data.getCriteriaMatrix(criteria);
    ldouble weight = data.getCriteriaWeight(criteria);
    PrometheeFunctionAdapter* function = data.getFunction(criteria);
    bool isMax = data.getIsMax(criteria);

    vector<ldouble> values;
    for(int line = 0; line < nlines; line++) {
      for(int column = 0; column < ncolumns; column++) {
        if(validPixels[line][column]) {
          values.push_back(matrix[line][column]);
        }
      }
    }

    sort(values.begin(), values.end());

    int nvalues = values.size();
    vector<ldouble> cummulative(nvalues, 0);

    cummulative[0] = values[0];
    for(int i = 1; i < nvalues; i++) {
      cummulative[i] = cummulative[i - 1] + values[i];
    }

    for(int line = 0; line < nlines; line++){
      for(int column = 0; column < ncolumns; column++){
        if(validPixels[line][column]){

          if(isMax){
            positiveFlow[line][column] += (*function).getPositiveDelta(values, matrix[line][column], cummulative, weight);
            negativeFlow[line][column] += (*function).getNegativeDelta(values, matrix[line][column], cummulative, weight);
          } else {
            negativeFlow[line][column] += (*function).getPositiveDelta(values, matrix[line][column], cummulative, weight);
            positiveFlow[line][column] += (*function).getNegativeDelta(values, matrix[line][column], cummulative, weight);
          }

        }
      }
    }
  }

  // applying a not standard normalization (but used by grass)
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++){
      positiveFlow[line][column] /= ncriterias;
      negativeFlow[line][column] /= ncriterias;
    }

  // calculating global flow
  for(int line = 0; line < nlines; line++) {
    for(int column = 0; column < ncolumns; column++) {
      netFlow[line][column] = positiveFlow[line][column] - negativeFlow[line][column];
    }
  }

  // generating results 
  PrometheeResult result = PrometheeResult();
  result.positiveFlow = positiveFlow;
  result.negativeFlow = negativeFlow;
  result.netFlow = netFlow;
  result.validPixels = validPixels;

  // normalizing results
  result.normalizedFlow = Normalizer().normalize(netFlow, validPixels);
  return result;  
}
