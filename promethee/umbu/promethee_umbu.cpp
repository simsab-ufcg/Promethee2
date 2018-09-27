#include "promethee_umbu.h"
#include "../parse_args.h"
#include "tiffio.h"
#include <iostream>
#include "../functions/umbu/linear_umbu_function.h"
#include "../functions/umbu/linear_with_indifference_umbu_function.h"
#include "tiffio.h"
#include "../plibtiff.h"
#include <map>
#include <cmath>

/**
./run -um AEMMF.tif 0.47 -chunk=1111 -ismax -type=linear (args...)
**/
void PrometheeUmbu::init(vector<string> args, int divideBy){
    this->divideBy = divideBy;
    string type = getCmdOption(args, "-type");
    if(!type.size() || (type != "linear" && type != "linearWithIndifference")){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    string chunk = getCmdOption(args, "-chunk");
    if(!chunk.size()){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    this->chunkBound = atoi(chunk.c_str());
    // cout << "chunk_bound " << this->chunkBound + 1 << endl;
    this->isMax = hasFlag(args, "-ismax");
    // cout << "is_max " << this->isMax << endl;
    this->filename = args[0];
    // cout << "filename " << this->filename << endl;
    this->weight = atof(args[1].c_str());
    // cout << "weight " << this->weight << endl;
    vector<ldouble> params;
    for(int i = 2; i < args.size(); i++)
        params.push_back(atof(args[i].c_str()));
    // cout << "params" << endl;
    // for(auto it : params) cout << it << " "; cout << endl;
    // cout << "type " << type << endl;
    if(type == "linear"){
        this->function = new LinearUmbuFunction(params);
    } else if(type == "linearWithIndifference"){
        this->function = new LinearWithIndifferenceUmbuFunction(params);
    }
}

void PrometheeUmbu::generateChunkOutTifUnbu(string &outputFile, string &nextFile, TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum){
    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile, this->width, this->height);

    ldouble *line = new ldouble[this->width];
    ldouble *outline = new ldouble[this->width];
    for (int i = 0; i < this->height; i++){

        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        for (int j = 0; j < this->width; j++) {
            if(line[j] < 0 || isnan(line[j]))
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            else
                outline[j] += (this->isMax ? 1 : -1) * ((*this->function).getPositiveDelta(values, line[j], sumAccum, weight, cntAccum) - (*this->function).getNegativeDelta(values, line[j], sumAccum, weight, cntAccum));
        }
	
        TIFFWriteScanline(nxt, outline, i);
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
}

void PrometheeUmbu::processChunk(map<double, int> & cnt, string & outputFile, string & nextFile, TIFF * input){
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
    this->generateChunkOutTifUnbu(outputFile, nextFile, input, values, sumAcc, countAcc);
}

void PrometheeUmbu::process(){

    TIFF *input = TIFFOpen(this->filename.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &this->samplePerPixel);

    string nextFile = "nxt." + this->filename;
    string outputFile = "out." + this->filename;

    setupOutput(outputFile, this->width, this->height);

    ldouble *line = new ldouble[width];
    map<double, int> cnt;
    
    int studyArea = 0;
    for(int i = 0; i < height; i++){
        TIFFReadScanline(input, line, i);
        for(int j = 0; j < width; j++){
            if(!isnan(line[j])){
                cnt[line[j]]++;
                studyArea++;
            }
        }
        if(cnt.size() >= this->chunkBound){
            this->processChunk(cnt, outputFile, nextFile, input);
        }
    }
    if(cnt.size() > 0){
        this->processChunk(cnt, outputFile, nextFile, input);
    }

    TIFFClose(input);
}