#include "tiffio.h"
#include <bits/stdc++.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

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
            int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            ldouble value = weight * ptr2;
            sum += value;

            int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
            int amount = ptr - ptr2;
            if(amount > 0){
                ldouble value = 0;
                if(ptr > 0) value += cummulative[ptr - 1];
                if(ptr2 > 0) value -= cummulative[ptr2 - 1];
                ldouble fvalue = weight * (amount * queryValue - value) / pParameter;
                sum += fvalue;
            }
        }
        return sum;
    }
    ldouble getNegativeDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight){
        ldouble sum = 0;
        {
            int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
            if(ptr2 < values.size()){
                ldouble value = weight * (values.size() - ptr2);
                sum += value;
            }
            
            int ptr = upper_bound(values.begin(), values.end(), queryValue) - values.begin();
            int amount = ptr2 - ptr;
            if(amount > 0){
                ldouble value = 0;
                if(ptr2 > 0) value += cummulative[ptr2 - 1];
                if(ptr > 0) value -= cummulative[ptr - 1];
                ldouble fvalue = weight * (value - amount * queryValue) / pParameter;
                sum += fvalue;        
            }
        }
        return sum;
    }
    ldouble getPositiveDelta(vector<ldouble> &values, ldouble queryValue, vector<ldouble> &cummulative, ldouble weight, vector<unsigned int> &cnt){
        ldouble sum = 0;
        {
            int ptr2 = lower_bound(values.begin(), values.end(), queryValue - pParameter) - values.begin();
            ldouble value = weight * (ptr2 > 0 ? cnt[ptr2 - 1] : 0);
            sum += value;
        
            int ptr = lower_bound(values.begin(), values.end(), queryValue) - values.begin();
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
            int ptr2 = upper_bound(values.begin(), values.end(), queryValue + pParameter) - values.begin();
            if (ptr2 < values.size()){
                ldouble value = weight * (cnt.back() - (ptr2 > 0 ? cnt[ptr2 - 1] : 0));
                sum += value;
            }

            int ptr = upper_bound(values.begin(), values.end(), queryValue) - values.begin();
            int amount = 0;
            if(ptr2 > 0) amount += cnt[ptr2 - 1];
            if(ptr > 0) amount -= cnt[ptr - 1];
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
    clock_gettime(CLOCK_REALTIME, &res);
    cout << res.tv_sec << " " << description.c_str() << " " << getpid() << endl;
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

    logger("FlowCalc start");
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
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
    logger("FlowCalc end");
}

void swapFiles(string filea, string fileb){
    if(rename(filea.c_str(), fileb.c_str()) == 0){
    }
}

/** 
 * ./run filename weight pparameter
**/
int main(int argc, char *argv[]){

    logger("Umbu start");
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

    logger("CreatingOutputFile start");

    setupOutput(outputFile);

    logger("CreatingOutputFile end");

    ldouble *line = new ldouble[width];
    int idx = 0;
    map<double, int> cnt;    
    logger("InitializeChunk-"+ to_string(idx) + " start");
    for(int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        for(int j = 0; j < width; j++)
            if(line[j] < 0 || isnan(line[j]));
            else
                cnt[line[j]]++;       
	if(cnt.size() > bound){
            logger("InitializeChunk-"+ to_string(idx) + " end");
	    logger("PreProcessChunk-"+ to_string(idx) + " start");
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
	    logger("PreProcessChunk-"+ to_string(idx) + " end");
            generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
	    idx++;
            logger("InitializeChunk-"+ to_string(idx) + " start");
        }
    }
    logger("InitializeChunk-"+ to_string(idx) + " end");
    if(cnt.size() > 0){
        logger("PreProcessChunk-"+ to_string(idx) + " start");
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
        logger("PreProcessChunk-"+ to_string(idx) + " end");
        generateChunkOutTifUnbu(input, values, sumAcc, countAcc);
        idx++;
    }

    if(outputFile[0] != 'o'){
        swapFiles(outputFile, nextFile);
    }

    TIFFClose(input);
    logger("Umbu end");
    return 0;
}
