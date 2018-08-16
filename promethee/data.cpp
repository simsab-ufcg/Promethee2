#include "data.h"
#include <math.h>

void Data::addCriteria(Matrix matrix, MatrixMetaData meta){
  matrices.push_back(matrix);
  metaData.push_back(meta);
}

Matrix Data::getCriteriaMatrix(int index){
  return matrices[index];
}

ldouble Data::getCriteriaWeight(int index){
  return metaData[index].weight;
}

bool Data::getIsMax(int index){
  return metaData[index].isMax;
}

PrometheeFunctionAdapter* Data::getFunction(int index){
  return metaData[index].function;
}

void Data::normalizeWeights(){
  ldouble weightsSum = 0;
  int ncriteria = metaData.size();
  for(int index = 0; index < ncriteria; index++)
    weightsSum += getCriteriaWeight(index);
  for(int index = 0; index < ncriteria; index++)
    metaData[index].weight /= weightsSum;
}

MaskMatrix Data::getMaskMatrix(){
  int nlines = matrices[0].size();
  int ncolumns = matrices[0][0].size();
  MaskMatrix mask = MaskMatrix(nlines, MaskMatrixLine(ncolumns, true));
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++)
      if(isnan(matrices[0][line][column]))
        mask[line][column] = false;
  return mask;
}
