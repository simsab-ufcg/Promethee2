#ifndef DATA_INCLUDED
  
  #define DATA_INCLUDED

  #include "types.h"
  #include "matrix_meta_data.h"
  #include "promethee_function_adapter.h"
  
  struct Data{
  
    vector<Matrix> matrices;
    vector<MatrixMetaData> metaData;
  
    void addCriteria(Matrix matrix, MatrixMetaData meta);
  
    Matrix getCriteriaMatrix(int index);
  
    ldouble getCriteriaWeight(int index);
  
    PrometheeFunctionAdapter* getFunction(int index);
  
    bool getIsMax(int index);
  
    void normalizeWeights();
  
    MaskMatrix getMaskMatrix();
  };
#endif
