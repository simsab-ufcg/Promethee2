#pragma once

#include "types.h"
#include "data.h"
#include "promethee_function_adapter.h"

struct argsThread {
    Matrix* input;
    MaskMatrix* validPixels;
    Matrix* positiveOut;
    Matrix* negativeOut;
    PrometheeFunctionAdapter * function;
    ldouble weight;
    bool isMax;
};

struct PrometheeThread {
    void work(argsThread args);
    PrometheeResult process(Data data);
};