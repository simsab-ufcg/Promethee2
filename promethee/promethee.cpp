#include "promethee.h"
#include "normalize.h"
#include <iostream>
PrometheeResult Promethee::process(Data data){

  int ncriterias = data.matrices.size();
  int nlines = data.matrices[0].size();
  int ncolumns = data.matrices[0][0].size();

  MaskMatrix validPixels = data.getMaskMatrix();

  Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));

  for(int criteria = 0; criteria < ncriterias; criteria++){

    Matrix matrix = data.getCriteriaMatrix(criteria);
    ldouble weight = data.getCriteriaWeight(criteria);
    vector<ldouble> values;

    for(int line = 0; line < nlines; line++)
      for(int column = 0; column < ncolumns; column++)
        if(validPixels[line][column])
          values.push_back(matrix[line][column]);

    sort(values.begin(), values.end());

    int nvalues = values.size();

    vector<ldouble> cummulative(nvalues, 0);

    cummulative[0] = values[0];
    for(int i = 1; i < nvalues; i++)
      cummulative[i] = cummulative[i - 1] + values[i];

    for(int line = 0; line < nlines; line++)
      for(int column = 0; column < ncolumns; column++){
        if(validPixels[line][column]){
          {
            int ptr = lower_bound(values.begin(), values.end(), matrix[line][column]) - values.begin();
            if(ptr > 0) {
              positiveFlow[line][column] += weight * (ptr * matrix[line][column] - cummulative[ptr - 1]);
            }
          }
          {
            int ptr = upper_bound(values.begin(), values.end(), matrix[line][column]) - values.begin();
            if(ptr < nvalues){
              ldouble cummulativePart = cummulative[nvalues - 1];
              if(ptr > 0)
                cummulativePart -= cummulative[ptr - 1];
              negativeFlow[line][column] += weight * (cummulativePart - (nvalues - ptr) * matrix[line][column]);
            }
          } 
        } else {
          // TODO
        }
      }
  }

  // not standart normalization, but used by grass
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++){
      positiveFlow[line][column] /= ncriterias;
      negativeFlow[line][column] /= ncriterias;
    }

  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++)
      netFlow[line][column] = positiveFlow[line][column] - negativeFlow[line][column];

  PrometheeResult result = PrometheeResult();
  result.positiveFlow = positiveFlow;
  result.negativeFlow = negativeFlow;
  result.netFlow = netFlow;
  result.validPixels = validPixels;
  result.normalizedFlow = Normalizer().normalize(netFlow, validPixels);
  return result;  
}
