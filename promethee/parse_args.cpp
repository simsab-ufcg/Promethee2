#include "parse_args.h"

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