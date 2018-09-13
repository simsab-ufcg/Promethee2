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
    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
        ldouble sum = 0;
        {
            int ptr = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            ldouble value = weight * ptr;
            sum += value;
        }
        {
            int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
            int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            int amount = 0;
            if(ptr > 0) amount += cnt[ptr - 1];
            if(ptr2 > 0) amount -= cnt[ptr2 - 1];
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
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
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
            int amount = 0;
            if(ptr > 0) amount += cnt[ptr - 1];
            if(ptr2 > 0) amount -= cnt[ptr2 - 1];
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
const int chunks = 16;

vector<ldouble> makeSumAccum(vector<ldouble> &values){
    vector<ldouble> result;
    ldouble sum = 0;
    for (ldouble &value : values){
        sum += value;
        result.push_back(sum);
    }
    return result;
}

int height, width, samplePerPixel,stripOffsets;

LinearFunction linear({0});

ldouble weight;
TIFF* openFile(string filename){
    TIFF* file = TIFFOpen(filename.c_str(), "wm");
    TIFFSetField(file, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(file, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, samplePerPixel);
    // TIFFSetField(file, TIFFTAG_STRIPOFFSETS, stripOffsets);
    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(file, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    return file;
}

void setupOutput(string outputFile){
    TIFF *out = openFile(outputFile);
    ldouble *line = new ldouble[width];
    for(int j = 0; j < width; j++)
        line[j] = 0;
    for(int i = 0; i < height; i++)
        TIFFWriteScanline(out, line, i);
    TIFFClose(out);
}

// output is the current output
// tmp is the next output
string outputFile, nextFile;
void generateChunkOutTif(TIFF *input, vector<ldouble> &values, vector<ldouble> &sumAccum){

    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile);

    ldouble *line = new ldouble[width];
    ldouble *outline = new ldouble[width];
    for (int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        for (int j = 0; j < width; j++) {
            if(line[j] < 0 || isnan(line[j]))
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            else
                outline[j] += linear.getPositiveDelta(values, line[j], sumAccum, weight) - linear.getNegativeDelta(values, line[j], sumAccum, weight);
        }
        TIFFWriteScanline(nxt, outline, i);
        if((i&127) == 0)
            cerr << i << endl;
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
}

void generateChunkOutTifUnbu(TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum){
    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile);

    ldouble *line = new ldouble[width];
    ldouble *outline = new ldouble[width];
    for (int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        for (int j = 0; j < width; j++) {
            if(line[j] < 0 || isnan(line[j]))
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            else
                outline[j] += linear.getPositiveDelta(values, line[j], sumAccum, weight, cntAccum) - linear.getNegativeDelta(values, line[j], sumAccum, weight, cntAccum);
        }
        TIFFWriteScanline(nxt, outline, i);
        if((i&127) == 0)
            cerr << i << endl;
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
}

/** 
 * ./run filename weight pparameter
**/
int main(int argc, char *argv[]){

    string criteriaName = argv[1];
    outputFile = "out." + criteriaName;
    nextFile = "nxt." + criteriaName;
    weight = atof(argv[2]);
    ldouble pparameter = atof(argv[3]);

    linear = LinearFunction({pparameter});

    TIFF *input = TIFFOpen(criteriaName.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
    // TIFFGetField(input, TIFFTAG_STRIPOFFSETS, &stripOffsets);

    setupOutput(outputFile);

    ldouble *line = new ldouble[width];

    // int chunkStart[chunks + 1];
    // for (int i = 0; i < chunks; i++)
    //     chunkStart[i] = (height / chunks) * i;
    // chunkStart[chunks] = height;

    cerr << "Image size: " << height << " " << width << endl;

    // for (int chunk = 0; chunk < chunks; chunk++){
    //     int start = chunkStart[chunk], end = chunkStart[chunk + 1] - 1;
    //     cerr << start << " " << end << endl;
    //     cerr << "reading" << endl;
    //     vector<ldouble> values;
    //     for (int i = start; i <= end; i++){
    //         TIFFReadScanline(input, line, i);
    //         for (int j = 0; j < width; j++){
    //             if(line[j] < 0 || isnan(line[j]));
    //             else
    //                 values.push_back(line[j]);
    //         }
    //         if((i&127) == 0) cerr << i << endl;
    //     }
    //     sort(values.begin(), values.end());
    //     vector<ldouble> sumAccum = makeSumAccum(values);
    //     cerr << "writing" << endl;
    //     generateChunkOutTif(input, values, sumAccum);
    // }

    map<double, int> cnt;
    for(int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        for(int j = 0; j < width; j++)
            if(line[j] < 0 || isnan(line[j]));
            else
                cnt[line[j]]++;
        if((i&127) == 0) cerr << "rd " << i << endl;
        if(cnt.size() > bound){
            cerr << "unbu-caja" << endl;
            vector<ldouble> values;
            for(auto it : cnt) values.push_back(it.first);
            vector<ldouble> sumAcc;
            ldouble sum = 0;
            for(auto &it : cnt) {
                sum += it.first * it.second;
                sumAcc.push_back(sum);
            }
            vector<unsigned int> countAcc;
            unsigned int scnt = 0;
            for(auto &it: cnt){
                scnt += it.second;
                countAcc.push_back(scnt);
            }
            cnt.clear();
            generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
        }
    }
    if(cnt.size() > 0){
        cerr << "unbu-caja" << endl;
        vector<ldouble> values;
        for(auto it : cnt) values.push_back(it.first);
        vector<ldouble> sumAcc;
        ldouble sum = 0;
        for(auto &it : cnt) {
            sum += it.first * it.second;
            sumAcc.push_back(sum);
        }
        vector<unsigned int> countAcc;
        unsigned int scnt = 0;
        for(auto &it: cnt){
            scnt += it.second;
            countAcc.push_back(scnt);
        }
        cnt.clear();
        generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
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