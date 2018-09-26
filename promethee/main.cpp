#include "types.h"
#include "data.h"
#include "normalize.h"
#include "optimized/promethee_opt.h"
#include "inputreader.h"
#include "outputwriter.h"
#include "vanilla/promethee_vanilla.h"
#include <iostream>
#include <dirent.h>

using namespace std;

void validateInput(Data data, int nMatrices) {
  int nLines = data.matrices[0].size(), nColumns;

  for(int i = 1; i < nMatrices; i++) {
    if(nLines != data.matrices[i].size()) {
      cerr << "Invalid input file (quantity of lines does not match)" << endl;
      exit(0);
    }

    for(int j = 0; j < nLines; j++) {
      nColumns = data.matrices[0][j].size();
      if(nColumns != data.matrices[i][j].size()) {
        cerr << "Invalid input file (quantity of columns does not match)" << endl;
        exit(0);
      }
    }
  }
}

vector<string> convertToVector(int argc, char * argv[]){
  vector<string> args;
  for(int i = 1; i < argc; i++) args.push_back(argv[i]);
  return args;
}

bool hasFlag(vector<string> & args, string flag){
  int size = args.size();
  for(int i = 0; i < size; i++){
    if(args[i] == flag){
      for(int j = i + 1; j < size; j++) args[j - 1] = args[j];
      args.resize(size - 1);
      return true;
    }
  }
  return false;
}

string getCmdOption(vector<string> & args, string cmd){
  int size = args.size();
  cmd = cmd + "=";
  for(int i = 0; i < size; i++){
    if(args[i].find(cmd) == 0){
      string res = args[i].substr(cmd.size());
      for(int j = i + 1; j < size; j++) args[j - 1] = args[j];
      args.resize(size - 1);
      return res;
    }
  }
  return "";
}

int main(int argc, char *argv[]){

  vector<string> args = convertToVector(argc, argv);

  bool isVan = hasFlag(args, "-van");
  bool isUmbu = hasFlag(args, "-um");
  bool isOpt = !isVan && !isOpt;
  
  string hq_flag = getCmdOption(args, "-hq");
  int divideBy = -1;

  if(hq_flag.size()){
    divideBy = atoi(hq_flag.c_str());
  }

  Promethee* res;
  if(isOpt)
    res = new PrometheeOpt();
  else if(isVan)
    res = new PrometheeVanilla();
  else if(isUmbu);

  res->init(args, divideBy);
  res->process();
  return 0;
}
