#pragma once

#include "../types.h"
#include "../data.h"
#include "../promethee.h"
#include <algorithm>
#include "../promethee_function_adapter.h"

struct argsThread {
    Matrix* input;
    MaskMatrix* validPixels;
    Matrix* positiveOut;
    Matrix* negativeOut;
    PrometheeFunctionAdapter * function;
    ldouble weight;
    bool isMax;
};

struct PrometheeThread : Promethee {

    string pathToOutput;
    vector<string> inputFiles, metaFiles;

    Data readData();
    void process();
    void init(vector<string> args, int divideBy);
};