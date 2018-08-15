#ifndef TYPES_INCLUDED

  #define TYPES_INCLUDED
  
  #include <vector>
  #include <limits>
  
  using namespace std;
  
  using ldouble = double;
  
  using MatrixLine = vector<double>;
  
  using Matrix = vector<MatrixLine>;
  
  using MaskMatrixLine = vector<bool>;
  
  using MaskMatrix = vector<MaskMatrixLine>;

  const ldouble NAN = std::numeric_limits<ldouble>::quiet_NaN();

  struct PrometheeResult{
    Matrix positiveFlow;
    Matrix negativeFlow;
    Matrix netFlow;
    MaskMatrix validPixels;
    Matrix normalizedFlow;
  };

#endif
