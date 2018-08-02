#ifndef DATA_INCLUDED
  #include "types.h"
  struct Data{
    vector<Matrix> matrices;
    vector<ldouble> weights;
    void addCriteria(ldouble weight, Matrix matrix);
    Matrix getCriteriaMatrix(int index);
    ldouble getCriteriaWeight(int index);
  };
#endif
#define DATA_INCLUDED
