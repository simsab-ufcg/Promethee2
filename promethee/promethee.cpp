#pragma once

#include "promethee.h"

Promethee::Promethee(){

}

void Promethee::init(vector<string> inputFiles, vector<string> metaFiles, string pathToOutput){
    this->inputFiles = inputFiles;
    this->metaFiles = metaFiles;
    this->pathToOutput = pathToOutput;
}

void Promethee::process(){

}