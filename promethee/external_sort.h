#pragma once

#include "plibtiff.h"
#include "utils.h"
#include "types.h"
#include <algorithm>
#include <assert.h>
#include <queue>
#include "parse_args.h"


struct ExternalSort {

    string path, positions;
    long long megaBytes, bucketAmount, bucketSize;
    TIFF *input, *output;
    int width, height, sampleFormat;
    int start, end;

    ExternalSort();
    ExternalSort(string path, long long megaBytes);

    void sort();
    void reverse();
    void main(vector<string> args);

private: 

    void k_wayMergesort(vector < pair < string, string> > paths);
    pair < string, string> parcialSort(int start, int end);
    void fillBuffer(TIFF *dataset, ldouble buffer[], PixelReader &pr, int line, int &size);
};