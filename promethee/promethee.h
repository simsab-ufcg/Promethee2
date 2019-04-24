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
    std::string pathToOutput;
    std::vector<std::string> inputFiles, metaFiles;

    int divideBy;

    Promethee();

    /**
     * Setup the promethee knowing the args
     * */
    virtual void init(std::vector<std::string> args, int divideBy) ;

    /**
     * Calculate the flow
     * */
    virtual void process();
};