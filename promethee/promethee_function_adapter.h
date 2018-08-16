#ifndef PROMETHEE_FUNCTION_ADAPTER_INCLUDED
  
  #define PROMETHEE_FUNCTION_ADAPTER_INCLUDED
  
  #include "types.h"
  #include "vanilla/comparison_function.h"
  #include "vanilla/usual_function.h"
  #include "vanilla/quasi_function.h"
  #include "vanilla/linear_function.h"
  #include "vanilla/level_function.h"
  #include "vanilla/linear_indifference_function.h"
  #include "vanilla/gaussian_function.h"
  #include "promethee_function.h"
  #include "linear_function.h"
  #include <string>

  struct PrometheeFunctionAdapter{
    PrometheeFunction* optFunction;
    ComparisonFunction* vanillaFunction;
    bool isOpt;

    PrometheeFunctionAdapter(bool isOpt, vector<ldouble> params, string funType){
      this->isOpt = isOpt;
      if(funType == "usual") {
        vanillaFunction = new UsualComparator(params);
      } else if(funType == "quasi") {
        vanillaFunction = new QuasiComparator(params);
      } else if(funType == "linear") {
        vanillaFunction = new LinearComparator(params);
      } else if(funType == "level") {
        vanillaFunction = new LevelComparator(params);
      } else if(funType == "linearWithIndifference") {
        vanillaFunction = new LinearWithIndifferenceComparator(params);
      } else if(funType == "gaussian") {
        vanillaFunction = new GaussianComparator(params);
      }
      optFunction = new LinearFunction(params);
    };

    ldouble getPositiveDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight);
    ldouble getNegativeDelta(vector<ldouble> & values, ldouble queryValue, vector<ldouble> & cummulative, ldouble weight);

    ComparisonFunction* getVanilla(){
      return this->vanillaFunction;
    }
  };
#endif