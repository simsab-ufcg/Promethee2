#include "types.h"
#include "data.h"
#include "normalize.h"
#include "promethee.h"
#include "inputreader.h"
#include "outputwriter.h"
#include "promethee_vanilla.h"
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

int main(int argc, char *argv[]){

  //lambdas
  auto validDir = [](string directory){
    if(directory.size() >= 1 && directory.back() != '/')
      return directory + '/';
    return directory;
  };

  auto endsWith = [](string text, string pattern){
    bool result = false;
    if(text.size() > pattern.size()){ 
      string match = text.substr(text.size() - pattern.size(), pattern.size());
      if(match == pattern)
        result = true;
    }
    return result;
  };

  auto readFiles = [](DIR* dir){
    vector<string> fileNames;
    struct dirent * directory;
    while((directory = readdir(dir)) != NULL){
      string fileName(directory->d_name);
      fileNames.push_back(fileName);
    }
    return fileNames;
  };

  auto filterDirectoryFiles = [readFiles, endsWith](string directoryName, string suffix){
    DIR* dirp = opendir(directoryName.c_str());
    vector<string> files;
    if(dirp){
      
      files = readFiles(dirp);
      
      auto iterator = remove_if(files.begin(), files.end(), [endsWith, suffix](string fileName){
        return ! endsWith(fileName, suffix);
      });
      
      files.erase(iterator, files.end());
      
      for(string & file : files)
        file = file.substr(0, file.size() - suffix.size());

    } else {
      cerr << "Directory " << directoryName.substr(0, directoryName.size() - 1) << " not found" << endl;
      exit(0);
    }
    
    closedir(dirp);
    return files;
  };

  //constants
  const string INPUT_FILE_SUFFIX = ".input";
  const string META_FILE_SUFFIX = ".meta";

  const int INPUT_DIRECTORY_INDEX = 1;
  const int META_DIRECTORY_INDEX = 2;
  const int OUTPUT_DIRECTORY_INDEX = 3;
  const int IS_OPT_FLAG_INDEX = 4;

  bool opt_flag = true;

  const string PATH_TO_OUTPUT_DIRECTORY(validDir(argv[OUTPUT_DIRECTORY_INDEX]));

  if(argc < 4 || argc > 5){
    cerr << "Arguments are invalid!";
    exit(0);
  }else if(argc == 5){
    if(string(argv[IS_OPT_FLAG_INDEX]) == "-V")
      opt_flag = false;
  }

  // argv in format (name_of_file, weight)
  Data data = Data();
  InputReader inputReader = InputReader();

  vector<string> valFiles, metaFiles;

  DIR           *dirp;
  struct dirent *directory;

  /* input directory */ 
  string inputDirectory(validDir(argv[INPUT_DIRECTORY_INDEX]));
  valFiles = filterDirectoryFiles(inputDirectory, INPUT_FILE_SUFFIX);

  /* meta directory */ 
  string metaDirectory(validDir(argv[META_DIRECTORY_INDEX]));
  metaFiles = filterDirectoryFiles(metaDirectory, META_FILE_SUFFIX);

  /* output directory */
  filterDirectoryFiles(PATH_TO_OUTPUT_DIRECTORY, "");

  sort(valFiles.begin(), valFiles.end());
  sort(metaFiles.begin(), metaFiles.end());

  if(valFiles != metaFiles) { // files don't match
    cout << "Error: not every file has its metadata accordingly or vice versa\n";
    return 0;
  }

  for(int i = 0; i < valFiles.size(); i++){

    string inputFile = inputDirectory + valFiles[i] + INPUT_FILE_SUFFIX;
    string metaFile = metaDirectory + metaFiles[i] + META_FILE_SUFFIX;

    cerr << "reading " << inputFile << endl;
    cerr << "reading " << metaFile << endl;

    // reading matrix of values in which each cell represent the pixel's value for a certain criteria
    Matrix nmatrix = inputReader.readMatrix(inputFile);
    MatrixMetaData metaData = inputReader.readMetaData(metaFile, opt_flag);

    // reading the criteria weight
    // ldouble weight = atof(argv[i + 1]);

    data.addCriteria(nmatrix, metaData);
  }

  validateInput(data, ((int) valFiles.size()));
  data.normalizeWeights();

  // applying promethee to the given data and generating its results
  PrometheeResult result;

  if(opt_flag){
    Promethee promethee = Promethee();
    result = promethee.process(data);
  } else {
    PrometheeVanilla promethee = PrometheeVanilla();
    result = promethee.process(data);
  }
  OutputWriter outputWriter = OutputWriter();

  // storing the generated result
  outputWriter.write(PATH_TO_OUTPUT_DIRECTORY, result);
  return 0;
}
