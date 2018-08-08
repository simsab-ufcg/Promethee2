#include "data.h"
void Data::addCriteria(ldouble weight, Matrix matrix, MatrixMetaData meta){
  matrices.push_back(matrix);
  weights.push_back(weight);
  metaData.push_back(meta);
}
Matrix Data::getCriteriaMatrix(int index){
  return matrices[index];
}
ldouble Data::getCriteriaWeight(int index){
  return weights[index];
}
bool Data::getIsMax(int index){
  return metaData[index].isMax;
}
PrometheeFunction* Data::getFunction(int index){
  return metaData[index].function;
}
void Data::normalizeWeights(){
  ldouble weightsSum = 0;
  for(ldouble weight : weights) 
    weightsSum += weight;
  for(ldouble & weight : weights) 
    weight /= weightsSum;
}
MaskMatrix Data::getMaskMatrix(){
  int nlines = matrices[0].size();
  int ncolumns = matrices[0][0].size();
  MaskMatrix mask = MaskMatrix(nlines, MaskMatrixLine(ncolumns, true));
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++)
      if(matrices[0][line][column] < 0)
        mask[line][column] = false;
  return mask;
}
