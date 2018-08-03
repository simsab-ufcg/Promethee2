#include "promethee.h"
#include <iostream>
// the result vector will contain netFlow, positiveFlow, negativeFlow, in the respective order
vector<Matrix> Promethee::process(Data data){

  int ncriterias = data.matrices.size();
  int nlines = data.matrices[0].size();
  int ncolumns = data.matrices[0][0].size();

  Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
  Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));

  cout << nlines << " " << ncolumns << endl;

  for(int criteria = 0; criteria < ncriterias; criteria++){

    Matrix matrix = data.getCriteriaMatrix(criteria);
    ldouble weight = data.getCriteriaWeight(criteria);
    vector<ldouble> values;

    for(int line = 0; line < nlines; line++)
      for(int column = 0; column < ncolumns; column++)
        if(matrix[line][column] >= 0)
          values.push_back(matrix[line][column]);

    sort(values.begin(), values.end());

    int nvalues = values.size();

    cout << (nlines * ncolumns) -  nvalues << endl;

    vector<ldouble> cummulative(nvalues, 0);

    cummulative[0] = values[0];
    for(int i = 1; i < nvalues; i++)
      cummulative[i] = cummulative[i - 1] + values[i];

    for(int line = 0; line < nlines; line++)
      for(int column = 0; column < ncolumns; column++){
        if(matrix[line][column] >= 0){
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


  vector<Matrix> result;
  result.push_back(netFlow);
  result.push_back(positiveFlow);
  result.push_back(negativeFlow);
  return result;
}
