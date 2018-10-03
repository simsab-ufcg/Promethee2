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

/**
 * Initialize required values to promethee umbu based in args
 * divideBy = -1 indicates that should be made the division by the (area_size - 1)
 * divideBy != -1 expected to be number of criterias
 * */
void PrometheeUmbu::init(vector<string> args, int divideBy){
    this->divideBy = divideBy;

    /**
     * Get type of function
     * */
    string type = getCmdOption(args, "-type");
    if(!type.size() || (type != "linear" && type != "linearWithIndifference")){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    
    /**
     * Get size of chunk
     * */
    string chunk = getCmdOption(args, "-chunk");
    if(!chunk.size()){
        cerr << "Error: incorrect arguments." << endl;
        exit(0);
    }
    this->chunkBound = atoi(chunk.c_str());

    this->isMax = hasFlag(args, "-ismax");

    /**
     * args[0] should be the filename by now, since there is no more flags
     * */
    this->filename = args[0];
    
    /**
     * args[1] is the weight
     * */
    this->weight = atof(args[1].c_str());

    /**
     * the rest of args is the arguments of the function
     * */
    vector<ldouble> params;
    for(int i = 2; i < args.size(); i++)
        params.push_back(atof(args[i].c_str()));

    /**
     * Setup function
     * */
    if(type == "linear"){
        this->function = new LinearUmbuFunction(params);
    } else if(type == "linearWithIndifference"){
        this->function = new LinearWithIndifferenceUmbuFunction(params);
    }
}

/**
 * Add contribution of a chunk to all alternatives
 * outputFile is where currently is the output
 * nextFile is to where the output should be sent
 * 
 * input, values, sumAccum, cnt are arguments to make calc of flow
 * */
void PrometheeUmbu::generateChunkOutTifUnbu(string &outputFile, string &nextFile, TIFF *input, vector<ldouble> & values, vector<ldouble> & sumAccum, vector<unsigned int> &cntAccum){
    
    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile, this->width, this->height);

    ldouble *line = new ldouble[this->width];
    ldouble *outline = new ldouble[this->width];
    for (int i = 0; i < this->height; i++){

        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        for (int j = 0; j < this->width; j++) {
            if(isnan(line[j])) {
                /**
                 * Write nan to correspond with input
                 * */
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            } else{
                ldouble positiveDelta = (*this->function).getPositiveDelta(values, line[j], sumAccum, weight, cntAccum);
                ldouble negativeDelta = (*this->function).getNegativeDelta(values, line[j], sumAccum, weight, cntAccum);
                /**
                 * if is not max function, just need to invert negative with positive
                 * */
                int mul = this->isMax ? 1 : -1;
                outline[j] += mul * (positiveDelta - negativeDelta);
            }
        }
	
        TIFFWriteScanline(nxt, outline, i);
    }
    TIFFClose(nxt);
    TIFFClose(out);

    /**
     * Output is swapped with the next file, 
     * to keep logic of filenaming
     * */
    swap(outputFile, nextFile);
}

/**
 * Makes division only after all the flows be defined, to run faster
 * */
void PrometheeUmbu::divide(string &outputFile, string &nextFile, TIFF *input){
    TIFF *out = TIFFOpen(outputFile.c_str(), "rm");
    TIFF *nxt = openFile(nextFile, this->width, this->height);

    ldouble *line = new ldouble[this->width];
    ldouble *outline = new ldouble[this->width];

    /**
     * 
     * */
    int denominator = (this->divideBy != -1 ? this->divideBy : this->area - 1);
    
    for (int i = 0; i < this->height; i++){

        TIFFReadScanline(input, line, i);
        TIFFReadScanline(out, outline, i);
        for (int j = 0; j < this->width; j++) {
            if(isnan(line[j])) {
                outline[j] += -sqrt(-1.0); // ?? this should be nan
            } else {
                outline[j] /= denominator;
            }
        }
	
        TIFFWriteScanline(nxt, outline, i);
    }
    TIFFClose(nxt);
    TIFFClose(out);
    swap(outputFile, nextFile);
}

/**
 * Preprocess required data based in map
 * */
void PrometheeUmbu::processChunk(map<double, int> & cnt, string & outputFile, string & nextFile, TIFF * input){
    /**
     * Get the keys of the map
     * */
    vector<ldouble> values;
    for(auto it : cnt)
        values.push_back(it.first);
    /**
     * Makes accumulated sum considering frequency of every element and its value
     * */
    vector<ldouble> sumAcc;
    ldouble sum = 0;
    for(auto &it : cnt) {
        sum += it.first * it.second;
        sumAcc.push_back(sum);
    }
    /**
     * Makes accumulated sum of the frequency of the elements
     * */
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

    /**
     * Open input and get fields need to create similar file for output
     * */
    TIFF *input = TIFFOpen(this->filename.c_str(), "rm");
    TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
    TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
    TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &this->samplePerPixel);

    /**
     * Name of files to use during processing
     * */
    string nextFile = "nxt." + this->filename;
    string outputFile = "out." + this->filename;

    /**
     * Create output file
     * */
    setupOutput(outputFile, this->width, this->height);

    ldouble *line = new ldouble[width];

    /**
     * To contabilize distinct values
     * */
    map<double, int> cnt;
    
    this->area = 0;
    for(int i = 0; i < height; i++){
        /**
         * Read line of input and add to map and area
         * */
        TIFFReadScanline(input, line, i);
        for(int j = 0; j < width; j++){
            if(!isnan(line[j])){
                cnt[line[j]]++;
                this->area++;
            }
        }
        if(cnt.size() >= this->chunkBound){
            /**
             * A chunk is full, time to add conribution
             * */
            this->processChunk(cnt, outputFile, nextFile, input);
        }
    }


    if(cnt.size() > 0){
        /**
         * An incomplete chunk, add his contribution
         * */
        this->processChunk(cnt, outputFile, nextFile, input);
    }

    /**
     * Divide
     * */
    this->divide(outputFile, nextFile, input);

    /**
     * Maybe after all processing the result be in nxt.filename,
     * to fix this we rename the file in case it happens,
     * with this always the result is in out.filename
     * */
    if(outputFile[0] != 'o'){
        rename(outputFile.c_str(), nextFile.c_str());
    }

    /**
     * 
     * */
    TIFFClose(input);
}