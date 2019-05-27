#pragma once

#include "../promethee.h"
#include "../functions/umbu/promethee_umbu_fun.h"
#include "tiffio.h"
#include "../utils.h"
#include <map>

/**
 * Structure of promethee umbu
 * */
struct PrometheeUmbu : Promethee {
    string filename; // name of criterion to be processed
    double weight; // weight of criterion
    uint16 sampleFormat; // size of bytes
    PrometheeUmbuFunction *function; // function to be used in processing
    int divideBy; // division constant , -1 to divide by (Area - 1)
    int chunkBound; // max chunk size
    bool isMax; // kind of function
    int area; // area of criterion
    int start, end; // first line and last line which will be processed
    // info related to criterion file
    int height, width, samplePerPixel;

    void init(vector<string> args, int divideBy);

    void process();

private:
    void processChunk(map<double, int> & cnt, string & outputFile, string & nextFile, TIFF * input);
    void generateChunkOutTifUnbu(string &outputFile, string &nextFile, TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum);
    void divide(string &output, string &nextFile, TIFF *input);
};