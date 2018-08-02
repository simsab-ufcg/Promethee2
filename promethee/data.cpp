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
