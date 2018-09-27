#include "promethee_umbu.h"
#include "../parse_args.h"
#include <iostream>

void PrometheeUmbu::init(vector<string> args, int divideBy){
    this->divideBy = divideBy;
    string type = getCmdOption(args, "-type");
    if(!type.size() || (type != "linear" && type != "linearWithIndifference")){
        cerr << "Error: incorrect arguments" << endl;
        exit(0);
    }
    string isMax = getCmdOption(args, "-isMax");
    
    filename = args[0];
    weigth = atof(args[1].c_str());
    
}