#include <vector>
#include <string>
using namespace std;

vector<string> convertToVector(int argc, char * argv[]);

bool hasFlag(vector<string> & args, string flag);

string getCmdOption(vector<string> & args, string cmd);