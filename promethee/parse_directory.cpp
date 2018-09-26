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