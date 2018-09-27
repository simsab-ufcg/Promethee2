#pragma once

#include "../promethee.h"
#include "../functions/umbu/promethee_umbu_fun.h"
#include "tiffio.h"
#include <map>

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
    void processChunk(map<double, int> & cnt, string & outputFile, string & nextFile, TIFF * input);
    void generateChunkOutTifUnbu(string &outputFile, string &nextFile, TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum);
};