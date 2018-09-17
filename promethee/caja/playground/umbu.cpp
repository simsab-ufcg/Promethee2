#include "tiffio.h"
#include <bits/stdc++.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
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

void logger(string description){
    timespec res;
    clock_gettime(CLOCK_MONOTONIC_RAW, &res);
    printf("%s(%d) %lld\n", description.c_str(), getpid(),  res.tv_sec*1000000000ll + res.tv_nsec);
}

const int bound = 30000000;
const int chunks = 12;

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
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
}

void generateChunkOutTifUnbu(TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum){

    logger("Generate-OpeningOutputFiles[start]");
    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile);
    logger("Generate-OpeningOutputFiles[end]");

    ldouble *line = new ldouble[width];
    ldouble *outline = new ldouble[width];
    for (int i = 0; i < height; i++){

        logger("Generate-OpeningInputFiles[start]");
        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        logger("Generate-OpeningInputFiles[end]");

        logger("Generate-flowCalc[start]");
        for (int j = 0; j < width; j++) {
            if(line[j] < 0 || isnan(line[j]))
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            else
                outline[j] += linear.getPositiveDelta(values, line[j], sumAccum, weight, cntAccum) - linear.getNegativeDelta(values, line[j], sumAccum, weight, cntAccum);
        }
        logger("Generate-flowCalc[end]");

        logger("Generate-WritingLineResult");
        TIFFWriteScanline(nxt, outline, i);
        logger("Generate-WritingLineResult");
    }

    logger("Generate-ClosingTiffs");
    TIFFClose(nxt);
    TIFFClose(out);
    logger("Generate-ClosingTiffs");
    swap(outputFile, nextFile);
}


/** 
 * ./run filename weight pparameter
**/
int main(int argc, char *argv[]){

    logger("Umbu[start]");

    string criteriaName = argv[1];
    outputFile = "out." + criteriaName;
    nextFile = "nxt." + criteriaName;
    weight = atof(argv[2]);
    ldouble pparameter = atof(argv[3]);

    linear = LinearFunction({pparameter});

    logger("OpeningInput[start]");

    TIFF *input = TIFFOpen(criteriaName.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
    // TIFFGetField(input, TIFFTAG_STRIPOFFSETS, &stripOffsets);

    logger("OpeningInput[end]");

    logger("CreatingOutputFile[start]");

    setupOutput(outputFile);

    logger("CreatingOutputFile[end]");

    ldouble *line = new ldouble[width];

    logger("LoopChunksProcessing[start]");
    map<double, int> cnt;
    for(int i = 0; i < height; i++){

        logger("ProcessLine[start]");
        // logger("ProcessLine_" + to_string(i) + "_" + to_string(height) + "_[start]");
        TIFFReadScanline(input, line, i);
        for(int j = 0; j < width; j++)
            if(line[j] < 0 || isnan(line[j]));
            else
                cnt[line[j]]++;
        logger("ProcessLine[end]");
        // logger("ProcessLine_" + to_string(i) + "_" + to_string(height) + "_[end]");
        if(cnt.size() > bound){
            logger("PreProcessChunk[start]");
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
            logger("PreProcessChunk[end]");
            logger("GeneratingOutputChunk[start]");
            generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
            logger("GeneratingOutputChunk[end]");
        }
    }
    logger("LoopChunksProcessing[end]");
    logger("IncompleteChunkProcessing[start]");
    if(cnt.size() > 0){
        logger("PreProcessChunk[start]");
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
        logger("PreProcessChunk[end]");
        logger("GeneratingOutputChunk[start]");
        generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
        logger("GeneratingOutputChunk[end]");
    }
    logger("IncompleteChunkProcessing[end]");

    TIFFClose(input);
    logger("Umbu[end]");
    return 0;
}
