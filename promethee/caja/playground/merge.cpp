#include "tiffio.h"
#include <bits/stdc++.h>

using namespace std;

/// ./run filename1 filename2 filename3 filename4
int main(int argc, char* argv[]){

    TIFF* tifs[argc - 1];
    for(int i = 1; i < argc; i++){
        cerr << "open " << string(argv[i]) << endl;
        tifs[i - 1] = TIFFOpen(argv[i], "rm");
    }

    int ntiffs = argc - 1;

    int width, height, sampleperpixel;

    TIFFGetField(tifs[0], TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tifs[0], TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tifs[0], TIFFTAG_SAMPLESPERPIXEL, &sampleperpixel);

    TIFF *out = TIFFOpen("merged.tif", "wm");
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);  // set the width of the image
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);    // set the height of the image
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channels per pixel
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    //   Some other essential fields to set that you do not have to understand for now.
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    double* data = new double[width];
    double* tmp = new double[width];
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++)
            data[j] = 0;
        for(int k = 0; k < ntiffs; k++){
            TIFFReadScanline(tifs[k], tmp, i);
            for(int j = 0; j < width; j++)
                data[j] += tmp[j];
        }
        // for(int j = 0; j < width; j++)
        //     data[j] /= ntiffs;
        TIFFWriteScanline(out, data, i);
    }

    TIFFClose(out);

    for(int i = 0; i < ntiffs; i++)
        TIFFClose(tifs[i]);
    return 0;
}