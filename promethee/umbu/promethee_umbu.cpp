#include "promethee_umbu.h"
#include "../parse_args.h"
#include <iostream>
#include "../functions/umbu/linear_umbu_function.h"
#include "../functions/umbu/linear_with_indifference_umbu_function.h"

/**
./run -um AEMMF.tif 0.47 -type=linear -chunk=1111 -ismax (args...)
**/
void PrometheeUmbu::init(vector<string> args, int divideBy){
    this->divideBy = divideBy;
    string type = getCmdOption(args, "-type");
    if(!type.size() || (type != "linear" && type != "linearWithIndifference")){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    string chunk = getCmdOption(args, "-chunk");
    if(!chunk.size()){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    this->chunkBound = atoi(chunk.c_str());
    // cout << "chunk_bound " << this->chunkBound + 1 << endl;
    this->isMax = hasFlag(args, "-ismax");
    // cout << "is_max " << this->isMax << endl;
    this->filename = args[0];
    // cout << "filename " << this->filename << endl;
    this->weight = atof(args[1].c_str());
    // cout << "weight " << this->weight << endl;
    vector<ldouble> params;
    for(int i = 2; i < args.size(); i++)
        params.push_back(atof(args[i].c_str()));
    // cout << "params" << endl;
    // for(auto it : params) cout << it << " "; cout << endl;
    // cout << "type " << type << endl;
    if(type == "linear"){
        this->function = new LinearUmbuFunction(params);
    } else if(type == "linearWithIndifference"){
        this->function = new LinearWithIndifferenceUmbuFunction(params);
    }
}