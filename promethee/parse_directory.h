#include <string>
#include <vector>
#include <dirent.h>
#include <iostream>
#include <algorithm>
using namespace std;

string validDir(string directory);

bool endsWith(string text, string pattern);

vector<string> readFiles(DIR* dir);

vector<string> filterDirectoryFiles(string directoryName, string suffix);