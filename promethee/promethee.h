#pragma once

#include <vector>
#include <string>
using namespace std;

struct Promethee{

    string pathToOutput;
    vector<string> inputFiles, metaFiles;
    int divideBy;

    Promethee();

    virtual void init(vector<string> inputFiles, vector<string> metaFiles, string pathToOutput, int divideBy) ;

    virtual void process();
};