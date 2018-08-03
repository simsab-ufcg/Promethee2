#ifndef TYPES_INCLUDED
  #include <vector>
  
  using namespace std;
  
  using ldouble = double;
  
  using MatrixLine = vector<double>;
  
  using Matrix = vector<MatrixLine>;
  
  using MaskMatrixLine = vector<bool>;
  
  using MaskMatrix = vector<MaskMatrixLine>;
  
  struct PrometheeResult{
    Matrix positiveFlow;
    Matrix negativeFlow;
    Matrix netFlow;
    MaskMatrix validPixels;
    Matrix normalizedFlow;
  };

#endif
#define TYPES_INCLUDED
