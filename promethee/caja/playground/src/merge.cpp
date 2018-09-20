#include "tiffio.h"
#include <bits/stdc++.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void logger(string description){
    timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    cout << res.tv_sec << " " << description.c_str() << " " << getpid() << endl;
}

/// ./run filename1 filename2 filename3 filename4
int main(int argc, char* argv[]){
    logger("merge start");
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
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH     , width); 
    TIFFSetField(out, TIFFTAG_IMAGELENGTH    , height);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(out, TIFFTAG_SAMPLEFORMAT  , 3);
    TIFFSetField(out, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(out, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP   , 8);
    TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(out, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(out, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );
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
    logger("merge end");
    TIFFClose(out);

    for(int i = 0; i < ntiffs; i++)
        TIFFClose(tifs[i]);
    return 0;
}
