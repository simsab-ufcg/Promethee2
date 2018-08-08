#ifndef DATA_INCLUDED
  #include "types.h"
  #include "matrix_meta_data.h"
  struct Data{
    vector<Matrix> matrices;
    vector<ldouble> weights;
    vector<MatrixMetaData> metaData;
    void addCriteria(ldouble weight, Matrix matrix, MatrixMetaData meta);
    Matrix getCriteriaMatrix(int index);
    ldouble getCriteriaWeight(int index);
    ldouble getPValue(int index);
    bool getIsMax(int index);
    void normalizeWeights();
    MaskMatrix getMaskMatrix();
  };
#endif
#define DATA_INCLUDED
