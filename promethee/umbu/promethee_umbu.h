#pragma once

#include "../promethee.h"
#include "../functions/umbu/promethee_umbu_fun.h"

struct PrometheeUmbu : Promethee {
    string filename;
    double weigth;
    int maxDistinctValues;
    PrometheeUmbuFunction *function;
    int divideBy;

    void init(vector<string> args, int divideBy);
};