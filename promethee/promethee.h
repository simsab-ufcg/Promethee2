#pragma once

#include <vector>
#include <string>
using namespace std;

struct Promethee{

    string pathToOutput;
    vector<string> inputFiles, metaFiles;
    int divideBy;

    Promethee();

    virtual void init(vector<string> args, int divideBy) ;

    virtual void process();
};