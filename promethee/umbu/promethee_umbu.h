#pragma once

#include "../promethee.h"
#include "../functions/umbu/promethee_umbu_fun.h"

struct PrometheeUmbu : Promethee {
    string filename;
    double weight;
    int maxDistinctValues;
    PrometheeUmbuFunction *function;
    int divideBy;
    int chunkBound;
    bool isMax;

    int height, width, samplePerPixel;

    void init(vector<string> args, int divideBy);

    void process();

private:
    
};