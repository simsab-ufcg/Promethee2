#include "optimized/promethee_opt.h"
#include "vanilla/promethee_vanilla.h"
#include "umbu/promethee_umbu.h"
#include "threads/promethee_thread.h"
#include "parse_args.h"

using namespace std;

int main(int argc, char *argv[]){

  vector<string> args = convertToVector(argc, argv);

  bool isVan = hasFlag(args, "-van");
  bool isUmbu = hasFlag(args, "-um");
  bool isThread = hasFlag(args, "-thr");
  bool isOpt = !isVan && !isUmbu && !isThread;
  
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
  else if(isUmbu)
    res = new PrometheeUmbu();
  else if(isThread)
    res = new PrometheeThread();

  res->init(args, divideBy);
  res->process();
  return 0;
}
