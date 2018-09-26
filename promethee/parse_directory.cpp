#include "parse_directory.h"

string validDir(string directory){
    if(directory.size() >= 1 && directory.back() != '/')
        return directory + '/';
    return directory;
};

bool endsWith(string text, string pattern){
    bool result = false;
    if(text.size() > pattern.size()){ 
        string match = text.substr(text.size() - pattern.size(), pattern.size());
        if(match == pattern)
            result = true;
    }
    return result;
}

vector<string> readFiles(DIR* dir){
    vector<string> fileNames;
    struct dirent * directory;
    while((directory = readdir(dir)) != NULL){
        string fileName(directory->d_name);
        fileNames.push_back(fileName);
    }
    return fileNames;
}

vector<string> filterDirectoryFiles(string directoryName, string suffix){
    DIR* dirp = opendir(directoryName.c_str());
    vector<string> files;
    if(dirp){
      
        files = readFiles(dirp);
      
        auto iterator = remove_if(files.begin(), files.end(), [&](string fileName){
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

void parseInputAndMeta(vector<string> args, vector<string> & inputFiles, vector<string> & metaFiles, string &outputDirectory) {
    const string INPUT_FILE_SUFFIX = ".input";
    const string META_FILE_SUFFIX = ".meta";

    const int INPUT_DIRECTORY_INDEX = 0;
    const int META_DIRECTORY_INDEX = 1;
    const int OUTPUT_DIRECTORY_INDEX = 2;

    string inputDirectory = validDir(args[INPUT_DIRECTORY_INDEX]);
    string metaDirectory = validDir(args[META_DIRECTORY_INDEX]);
    outputDirectory = validDir(args[OUTPUT_DIRECTORY_INDEX]);

    inputFiles = filterDirectoryFiles(inputDirectory, INPUT_FILE_SUFFIX);
    metaFiles = filterDirectoryFiles(metaDirectory, META_FILE_SUFFIX);

    sort(inputFiles.begin(), inputFiles.end());
    sort(metaFiles.begin(), metaFiles.end());

    if (inputFiles != metaFiles){ // files don't match
        cerr << "Error: not every file has its metadata accordingly or vice versa\n";
        exit(0);
    }

    for (int i = 0; i < inputFiles.size(); i++){
        inputFiles[i] = inputDirectory + inputFiles[i] + INPUT_FILE_SUFFIX;
        metaFiles[i] = metaDirectory + metaFiles[i] + META_FILE_SUFFIX;
    }
}