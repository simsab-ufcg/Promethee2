#include "tiffio.h"
#include <string>
using namespace std;

TIFF* openFile(string filename, int width, int height);

void setupOutput(string outputFile, int width, int height);