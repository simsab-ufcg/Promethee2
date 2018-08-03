#include "types.h"
#include "data.h"
#include "normalize.h"
#include "promethee.h"
#include "inputreader.h"
#include "outputwriter.h"
const string pathToInputDirectory = "promethee/input/";
const string pathToOutputDirectory = "promethee/output/";
int main(int argc, char *argv[]){
  // argv in format (name_of_file, weight)
  Data data = Data();
  InputReader inputReader = InputReader();
  for(int i = 1; i < argc; i += 2){
    string fileName(argv[i]);
    Matrix nmatrix = inputReader.readMatrix(pathToInputDirectory + fileName);
    ldouble weight = atof(argv[i + 1]);
    data.addCriteria(weight, nmatrix);
  }
  Promethee promethee = Promethee();
  PrometheeResult result = promethee.process(data);
  OutputWriter outputWriter = OutputWriter();
  outputWriter.write(pathToOutputDirectory, result);
  return 0;
}
