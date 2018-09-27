#include "tiffio.h"
#include <bits/stdc++.h>
using namespace std;

using ldouble = double;

using MatrixLine = vector<double>;

using Matrix = vector<MatrixLine>;

using MaskMatrixLine = vector<bool>;

using MaskMatrix = vector<MaskMatrixLine>;

// const ldouble NAN = std::numeric_limits<ldouble>::quiet_NaN();

struct LinearFunction{
    ldouble pParameter;

    LinearFunction(vector<ldouble> pParameter){
        this->pParameter = pParameter[0];
    }

    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight){
        ldouble sum = 0;
        {
            int ptr = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            ldouble value = weight * ptr;
            sum += value;
        }
        {
            int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
            int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            int amount = ptr - ptr2;
            if (amount > 0){
                ldouble value = 0;
                if (ptr > 0)
                    value += cummulative[ptr - 1];
                if (ptr2 > 0)
                    value -= cummulative[ptr2 - 1];
                ldouble fvalue = weight * (amount * queryValue - value) / pParameter;
                sum += fvalue;
            }
        }
        return sum;
    }
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight){
        ldouble sum = 0;
        {
            int ptr = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
            if (ptr < values.size()){
                ldouble value = weight * (values.size() - ptr);
                sum += value;
            }
        }
        {
            int ptr = upper_bound(values.begin(), values.end(), queryValue) - values.begin();
            int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
            int amount = ptr2 - ptr;
            if (amount > 0){
                ldouble value = 0;
                if (ptr2 > 0)
                    value += cummulative[ptr2 - 1];
                if (ptr > 0)
                    value -= cummulative[ptr - 1];
                ldouble fvalue = weight * (value - amount * queryValue) / pParameter;
                sum += fvalue;
            }
        }
        return sum;
    }
    ldouble getPParameter(){
        return pParameter;
    }
};

const int bound = 30000000;
const int chunks = 5;

vector<ldouble> makeSumAccum(vector<ldouble> &values){
    vector<ldouble> result;
    ldouble sum = 0;
    for (ldouble &value : values){
        sum += value;
        result.push_back(sum);
    }
    return result;
}

int height, width, samplePerPixel;

LinearFunction linear({0});

ldouble weight;

void generateChunkOutTif(TIFF *input, string name, int chunkId, vector<ldouble> &values, vector<ldouble> &sumAccum){
    string outName = "out" + to_string(chunkId) + "." + name;
    TIFF *out = TIFFOpen(outName.c_str(), "wm");
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, samplePerPixel);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    ldouble *line = new ldouble[width];
    ldouble *outline = new ldouble[width];
    for (int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        for (int j = 0; j < width; j++) {
            if(line[j] < 0)
                outline[j] = -sqrt(-1.0); // ?? this should be nan
            else
                outline[j] = linear.getPositiveDelta(values, line[j], sumAccum, weight);
        }
        TIFFWriteScanline(out, outline, i);
    }
    TIFFClose(out);
}

/** 
 * ./run filename weight pparameter
**/
int main(int argc, char *argv[]){

    string criteriaName = argv[1];
    weight = atof(argv[2]);
    ldouble pparameter = atof(argv[3]);

    linear = LinearFunction({pparameter});

    TIFF *input = TIFFOpen(criteriaName.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);

    ldouble *line = new ldouble[width];

    int chunkStart[chunks + 1];
    for (int i = 0; i < chunks; i++)
        chunkStart[i] = (height / chunks) * i;
    chunkStart[chunks] = height;

    cerr << "Image size: " << height << " " << width << endl;

    for (int chunk = 0; chunk < chunks; chunk++){
        int start = chunkStart[chunk], end = chunkStart[chunk + 1] - 1;
        cerr << start << " " << end << endl;
        vector<ldouble> values;
        for (int i = start; i <= end; i++){
            TIFFReadScanline(input, line, i);
            for (int j = 0; j < width; j++){
                if(line[j] < 0);
                else
                    values.push_back(line[j]);
            }
        }
        sort(values.begin(), values.end());
        vector<ldouble> sumAccum = makeSumAccum(values);
        generateChunkOutTif(input, criteriaName, chunk, values, sumAccum);
    }

    TIFFClose(input);
    // map<double, int> cnt;
    // //free(data);

    // vector<ldouble> values;

    // for(auto & it : cnt) values.push_back(it.first);

    // int nvalues = cnt.size();
    // vector<ldouble> cummulative(nvalues, 0);
    // vector<int> counter(nvalues, 0);

    // ldouble curSum = 0;

    // int ptr = 0;
    // int sdk = 0;
    // for(auto & it : cnt){
    //     curSum += it.first * it.second;
    //     cummulative[ptr] = curSum;
    //     sdk += it.second;
    //     counter[ptr++] = sdk;
    // }

    // LinearFunction linear = LinearFunction({1});

    // double wei = 0;

    // string lnd = string(argv[1]);

    // if(lnd[1] == 'V') wei = 0.53;
    // else wei = 0.47;

    // cerr << lnd << " " << wei << endl;

    // TIFF *tif2 = TIFFOpen(("u" + string(argv[1])).c_str(), "wm");
    // TIFFSetField (tif2, TIFFTAG_IMAGEWIDTH, width);  // set the width of the image
    // TIFFSetField(tif2, TIFFTAG_IMAGELENGTH, height);    // set the height of the image
    // TIFFSetField(tif2, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channels per pixel
    // TIFFSetField(tif2, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
    // TIFFSetField(tif2, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    // //   Some other essential fields to set that you do not have to understand for now.
    // TIFFSetField(tif2, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    // TIFFSetField(tif2, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    // // for(int row = 0; row < height; row++){
    // //     for(int i = 0; i < width; i++){
    // //         data[i] = 0;
    // //     }
    // //     TIFFWriteScanline(tif, data, row);
    // // }

    // cout << setprecision(8) << fixed;
    // for(int row = 0; row < height; row++){
    //     TIFFReadScanline(tif, data, row);
    //     for(int i = 0; i < width; i++){
    //         if(!(data[i] < 0)){
    //             out[i] = linear.getPositiveDelta(values, data[i], counter, cummulative, wei);
    //         } else {
    //             out[i] = -sqrt(-1.0);
    //         }
    //     }
    //     TIFFWriteScanline(tif2, out, row);
    // }

    // free(data);
    // TIFFClose(tif);
    // TIFFClose(tif2);
    return 0;
}