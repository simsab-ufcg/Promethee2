#include "plibtiff.h"
#include "types.h"

TIFF* openFile(string filename, int width, int height){
    TIFF* file = TIFFOpen(filename.c_str(), "w8m");
    TIFFSetField(file, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(file, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(file, TIFFTAG_SAMPLEFORMAT  , 3);
    TIFFSetField(file, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(file, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(file, TIFFTAG_ROWSPERSTRIP   , 8);
    TIFFSetField(file, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(file, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(file, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );
    return file;
}

void setupOutput(string outputFile, int width, int height){
    TIFF *out = openFile(outputFile, width, height);
    ldouble *line = new ldouble[width];
    for(int j = 0; j < width; j++)
        line[j] = 0;
    for(int i = 0; i < height; i++)
        TIFFWriteScanline(out, line, i);
    TIFFClose(out);
}