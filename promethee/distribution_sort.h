#pragma once

#include "plibtiff.h"
#include "utils.h"
#include "types.h"
#include <algorithm>
#include <assert.h>
#include <queue>
#include "buffer_manager.h"
#include "parse_args.h"


struct DistributionSort {
    
    string path, positions;
    long long megaBytes, bucketAmount, bucketSize;
    BufferManager input, output;
    int width, height, sampleFormat;
    int start, end;

    DistributionSort();
    DistributionSort(string path, long long megaBytes);
    const int TODO_CONSTANT = 1024;
    void sort();
    void main(vector<string> args);

private: 
    void sweepMerge(vector < pair < string, string> > paths);
};