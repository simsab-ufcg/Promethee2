#include "types.h"
#include "types.h"
#include "data.h"
#include "normalize.h"
#include "promethee.h"
#include "inputreader.h"
#include "outputwriter.h"
#include <iostream>
#include <dirent.h>
using namespace std;
const string pathToInputDirectory = "promethee/input/";

int main(int argc, char *argv[]){

  const string pathToOutputDirectory(argv[3]);

  // argv in format (name_of_file, weight)
  Data data = Data();
  InputReader inputReader = InputReader();

  vector<string> valFiles, metaFiles;

  DIR           *dirp;
  struct dirent *directory;

  for (int i = 1; i < argc; i++) {
    
    dirp = opendir(argv[i]);
    if (dirp) {   
      while ((directory = readdir(dirp)) != NULL) {

        string fileName(directory->d_name);
        if(fileName != "." && fileName != ".." && fileName != ".DS_Store") {
          if (i == 1) {                     // path to values must be the first one
            valFiles.push_back(fileName);
          } else if (i == 2) {              // path to metadata must be the second one
            metaFiles.push_back(fileName);
          } 
        }
      }
      closedir(dirp);
    } else {
      cout << "Directory " << argv[i] << "not found " << endl;
      return 0;
    }
  }

  sort(valFiles.begin(), valFiles.end());
  sort(metaFiles.begin(), metaFiles.end());

  if(valFiles != metaFiles) { // files dont match
    cout << "Error: not every file has its metadata accordingly or vice versa\n";
    return 0;
  }

  // return 0;
  for(int i = 0; i < valFiles.size(); i++){
    string valuePath(argv[1]);
    string metaPath(argv[2]);


    cout << valuePath + valFiles[i] << endl;
    cout << metaPath + metaFiles[i] << endl;

    // reading matrix of values in which each cell represent the pixel's value for a certain criteria
    Matrix nmatrix = inputReader.readMatrix(valuePath + valFiles[i]);
    MatrixMetaData metaData = inputReader.readMetaData(metaPath + metaFiles[i]);

    // reading the criteria weight
    // ldouble weight = atof(argv[i + 1]);

    data.addCriteria(nmatrix, metaData);
  }

  data.normalizeWeights();

  Promethee promethee = Promethee();

  // applying promethee to the given data and generating its results
  PrometheeResult result = promethee.process(data);
  OutputWriter outputWriter = OutputWriter();

  // storing the generated result
  outputWriter.write(pathToOutputDirectory, result);
  return 0;
}
