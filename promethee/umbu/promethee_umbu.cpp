#include "promethee_umbu.h"
#include "../parse_args.h"

void PrometheeUmbu::init(vector<string> args, int divideBy){
    this->divideBy = divideBy;
    filename = args[0];
    weigth = atof(args[1].c_str());

}