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


int main(int argc, char *argv[]){

  const string INPUT_FILE_SUFFIX = ".input";
  const string META_FILE_SUFFIX = ".meta";

  const int INPUT_DIRECTORY_INDEX = 1;
  const int META_DIRECTORY_INDEX = 2;
  const int OUTPUT_DIRECTORY_INDEX = 3;

  const string PATH_TO_OUTPUT_DIRECTORY(argv[OUTPUT_DIRECTORY_INDEX]);

  // argv in format (name_of_file, weight)
  Data data = Data();
  InputReader inputReader = InputReader();

  vector<string> valFiles, metaFiles;

  DIR           *dirp;
  struct dirent *directory;

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

  /* input directory */ 
  string inputDirectory(argv[INPUT_DIRECTORY_INDEX]);
  valFiles = filterDirectoryFiles(inputDirectory, INPUT_FILE_SUFFIX);

  /* meta directory */ 
  string metaDirectory(argv[META_DIRECTORY_INDEX]);
  metaFiles = filterDirectoryFiles(metaDirectory, META_FILE_SUFFIX);

  /* output directory */
  filterDirectoryFiles(PATH_TO_OUTPUT_DIRECTORY, "");

  sort(valFiles.begin(), valFiles.end());
  sort(metaFiles.begin(), metaFiles.end());

  if(valFiles != metaFiles) { // files dont match
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
    MatrixMetaData metaData = inputReader.readMetaData(metaFile);

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
  outputWriter.write(PATH_TO_OUTPUT_DIRECTORY, result);
  return 0;
}
