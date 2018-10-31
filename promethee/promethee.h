#pragma once

#include <vector>
#include <string>
using namespace std;

/**
 * Abstract structure of a promethee type
 * */
struct Promethee{

    /**
     * Input and output related data
     * */
    string pathToOutput;
    vector<string> inputFiles, metaFiles;

    int divideBy;

    Promethee();

    /**
     * Setup the promethee knowing the args
     * */
    virtual void init(vector<string> args, int divideBy) ;

    /**
     * Calculate the flow
     * */
    virtual void process();
};