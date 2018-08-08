#include "promethee.h"
#include "normalize.h"
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
    ldouble pvalue = data.getPValue(criteria);
    bool isMax = data.getIsMax(criteria);
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

    for(int line = 0; line < nlines; line++){
      for(int column = 0; column < ncolumns; column++){
        if(validPixels[line][column]){

          for(auto v : values){
            continue;
            ldouble diff = matrix[line][column] - v;
            diff = min(diff, pvalue);
            diff = max(diff, -pvalue);
            if(isMax){
              if(diff > 0)
                positiveFlow[line][column] += weight * diff / pvalue;
              else
                negativeFlow[line][column] += - weight * diff / pvalue;
            } else {
              if(diff < 0)
                positiveFlow[line][column] += - weight * diff / pvalue;
              else
                negativeFlow[line][column] += weight * diff / pvalue;
            }
          }
          // continue;
          {
            int ptr = lower_bound(values.begin(), values.end(), matrix[line][column] - pvalue) - values.begin();
            ldouble value = weight * ptr;
            if(line + column == 0) cout << value << " ";
            if(isMax)
              positiveFlow[line][column] += value;
            else
              negativeFlow[line][column] += value;
          }
          {
            int ptr = lower_bound(values.begin(), values.end(), matrix[line][column]) - values.begin();
            int ptr2 = lower_bound(values.begin(), values.end(), matrix[line][column] - pvalue) - values.begin();
            int amount = ptr - ptr2;
            if(amount > 0){
              ldouble value = 0;
              if(ptr > 0) value += cummulative[ptr - 1];
              if(ptr2 > 0) value -= cummulative[ptr2 - 1];
              ldouble fvalue = weight * (amount * matrix[line][column] - value) / pvalue;
              if(line + column == 0)cout << fvalue << " ";
              if(isMax)
                positiveFlow[line][column] += fvalue;
              else
                negativeFlow[line][column] += fvalue;
              
            }
          }
          {
            int ptr = upper_bound(values.begin(), values.end(), matrix[line][column] + pvalue) - values.begin();
            if(ptr < nvalues){
              ldouble value = weight * (nvalues - ptr);
              if(line + column == 0)cout << value << " ";
              if(isMax)
                negativeFlow[line][column] += value;
              else
                positiveFlow[line][column] += value;
            }
          }
          {
            int ptr = upper_bound(values.begin(), values.end(), matrix[line][column]) - values.begin();
            int ptr2 = upper_bound(values.begin(), values.end(), matrix[line][column] + pvalue) - values.begin();
            int amount = ptr2 - ptr;
            if(amount > 0){
              ldouble value = 0;
              if(ptr2 > 0) value += cummulative[ptr2 - 1];
              if(ptr > 0) value -= cummulative[ptr - 1];
              ldouble fvalue = weight * (value - amount * matrix[line][column]) / pvalue;
              if(line + column == 0)cout << fvalue << " ";
              if(isMax)
                negativeFlow[line][column] += fvalue;
              else
                positiveFlow[line][column] += fvalue;          
            }
          }
          if(line + column == 0) cout << endl;
        }
      }
    }
  }

  // applying a not standard normalization (but used by grass)
  // for(int line = 0; line < nlines; line++)
  //   for(int column = 0; column < ncolumns; column++){
  //     positiveFlow[line][column] /= ncriterias;
  //     negativeFlow[line][column] /= ncriterias;
  //   }

  // calculating global flow
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++)
      netFlow[line][column] = positiveFlow[line][column] - negativeFlow[line][column];

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
