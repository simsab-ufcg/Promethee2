#pragma once

#include "promethee.h"

Promethee::Promethee(){

}

void Promethee::init(vector<string> inputFiles, vector<string> metaFiles, string pathToOutput, int divideBy){
    this->inputFiles = inputFiles;
    this->metaFiles = metaFiles;
    this->pathToOutput = pathToOutput;
    this->divideBy = divideBy;
}

void Promethee::process(){

}