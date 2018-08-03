#include "data.h"
void Data::addCriteria(ldouble weight, Matrix matrix){
  matrices.push_back(matrix);
  weights.push_back(weight);
}
Matrix Data::getCriteriaMatrix(int index){
  return matrices[index];
}
ldouble Data::getCriteriaWeight(int index){
  return weights[index];
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
