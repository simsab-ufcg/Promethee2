#pragma once

#include "../promethee.h"
#include "tiffio.h"
#include "../utils.h"
#include <map>
#include "../external_sort.h"
#include "../buffer_manage.h"

/**
 * Structure of promethee fast
 * */

struct PrometheeFast : Promethee {

    BufferManager image;
    string filename; // name of criterion to be processed
    double weight; // weight of criterion
    int divideBy; // division constant , -1 to divide by (Area - 1)
    bool isMax; // kind of function
    int start, end; // first line and last line which will be processed
    double p, q; // Arguments of promethee function
    ExternalSort es;


    // info related to criterion file
    int height, width;

    void init(vector<string> args, int divideBy);

    void process();

private:
    void findLast(int &beginLine, int &beginColunm, ldouble const& firstValue);
    void findFirst(int &beginLine, int &beginColunm, ldouble const& firstValue);
};