#pragma once

#include "../promethee.h"
#include "tiffio.h"
#include "../utils.h"
#include <map>
#include "../external_sort.h"

/**
 * Structure of promethee fast
 * */

struct PrometheeFast : Promethee {

    TIFF *input;
    string filename; // name of criterion to be processed
    double weight; // weight of criterion
    uint16 sampleFormat; // size of bytes
    int divideBy; // division constant , -1 to divide by (Area - 1)
    bool isMax; // kind of function
    int start, end; // first line and last line which will be processed
    double p, q; // Arguments of promethee function
    ExternalSort es;


    // info related to criterion file
    int height, width, samplePerPixel;

    void init(vector<string> args, int divideBy);

    void process();

private:
    void findLast(PixelReader &pr, int &beginLine, int &beginColunm, ldouble const& firstValue);
    void findFirst(PixelReader &pr, int &beginLine, int &beginColunm, ldouble const& firstValue);
    void fillBuffer(ldouble buffer[], PixelReader &pr, int line);
};