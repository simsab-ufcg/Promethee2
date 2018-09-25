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

  const int INPUT_DIRECTORY_INDEX = 0;
  const int META_DIRECTORY_INDEX = 1;
  const int OUTPUT_DIRECTORY_INDEX = 2;

  vector<string> args = convertToVector(argc, argv);

  bool opt_flag = !hasFlag(args, "-V");
  bool hq_flag = hasFlag(args, "-HQ");

  const string PATH_TO_OUTPUT_DIRECTORY = validDir(args[OUTPUT_DIRECTORY_INDEX]);

  if(args.size() != 3){
    cerr << "Arguments are invalid!";
    exit(0);
  }

  // argv in format (name_of_file, weight)
  Data data = Data();
  InputReader inputReader = InputReader();

  vector<string> valFiles, metaFiles;

  DIR           *dirp;
  struct dirent *directory;

  /* input directory */ 
  string inputDirectory = validDir(args[INPUT_DIRECTORY_INDEX]);
  valFiles = filterDirectoryFiles(inputDirectory, INPUT_FILE_SUFFIX);

  /* meta directory */ 
  string metaDirectory = validDir(args[META_DIRECTORY_INDEX]);
  metaFiles = filterDirectoryFiles(metaDirectory, META_FILE_SUFFIX);

  cout << inputDirectory << " " << metaDirectory << " " << PATH_TO_OUTPUT_DIRECTORY << endl;
  /* output directory */
  filterDirectoryFiles(PATH_TO_OUTPUT_DIRECTORY, "");

  sort(valFiles.begin(), valFiles.end());
  sort(metaFiles.begin(), metaFiles.end());

  if(valFiles != metaFiles) { // files don't match
    cout << "Error: not every file has its metadata accordingly or vice versa\n";
    return 0;
  }

  for(int i = 0; i < valFiles.size(); i++){
    valFiles[i] = inputDirectory + valFiles[i] + INPUT_FILE_SUFFIX;
    metaFiles[i] = metaDirectory + metaFiles[i] + META_FILE_SUFFIX;
  }

  Promethee* res;
  if(opt_flag)
    res = new PrometheeOpt();
  else
    res = new PrometheeVanilla();
  res->init(valFiles, metaFiles, PATH_TO_OUTPUT_DIRECTORY);
  res->process();
  return 0;
}
