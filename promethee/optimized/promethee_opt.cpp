#include "promethee_opt.h"
#include "../normalize.h"
#include "../promethee_function_adapter.h"
#include <iostream>
#include "../functions/optimized/linear_function.h"
#include "../functions/optimized/linear_with_indifference_function.h"
#include "../inputreader.h"
#include "../outputwriter.h"
#include "../plibtiff.h"
#include "../parse_directory.h"
#include "../parse_args.h"

void PrometheeOpt::init(vector<string> args, int divideBy){
  this->divideBy = divideBy;
  
  // Get type of function
  string type = getCmdOption(args, "-type");
  if(!type.size() || (type != "linear" && type != "linearWithIndifference")){
      cerr << "Error: incorrect arguments." << endl;
      exit(0);
  }

  this->isMax = hasFlag(args, "-ismax");

  // args[0] should be the filename by now, since there is no more flags
  this->filename = args[0];

  // args[1] is the weight
  this->weight = atof(args[1].c_str());

  // the remaining args are function args
  vector<ldouble> params;
  for(int i = 2; i < args.size(); i++)
      params.push_back(atof(args[i].c_str()));

  // build specific funcion implementation
  if(type == "linear"){
      this->function = new LinearFunction(params);
  } else if(type == "linearWithIndifference"){
      this->function = new LinearWithIndifferenceFunction(params);
  }
}


void PrometheeOpt::process() {

  // Open input and get fields need to create similar file for output
  TIFF *input = TIFFOpen(this->filename.c_str(), "rm");
  TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
  TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
  TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
  TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &this->samplePerPixel);

  Matrix netFlow = Matrix(this->height, MatrixLine(this->width, 0.0));

  // Create PixelReader Interface
  unsigned short byte_size = TIFFScanlineSize(input) / this->width;
  tdata_t line = _TIFFmalloc(TIFFScanlineSize(input));
  PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);

  vector<ldouble> values;
  for(int i = 0; i < this->height; i++) {
    TIFFReadScanline(input, line, i);
    for(int j = 0; j < this->width; j++) {
      double aux = pr.readPixel(j);
      if(!isnan(aux)) {
        values.push_back(aux);
      }
    }
  }

  sort(values.begin(), values.end());

  int nvalues = values.size();
  vector<ldouble> cummulative(nvalues, 0);

  cummulative[0] = values[0];
  for(int i = 1; i < nvalues; i++) {
    cummulative[i] = cummulative[i - 1] + values[i];
  }

  for(int i = 0; i < this->height; i++){
    TIFFReadScanline(input, line, i);
    for(int j = 0; j < this->width; j++){
      if(!isnan(pr.readPixel(j))){

        if(this->isMax){
          netFlow[i][j] += (*this->function).getPositiveDelta(values, pr.readPixel(j), cummulative, this->weight);
          netFlow[i][j] -= (*this->function).getNegativeDelta(values, pr.readPixel(j), cummulative, this->weight);
        } else {
          netFlow[i][j] -= (*this->function).getPositiveDelta(values, pr.readPixel(j), cummulative, this->weight);
          netFlow[i][j] += (*this->function).getNegativeDelta(values, pr.readPixel(j), cummulative, this->weight);
        }

      }
    }
  }

  int denominator = (this->divideBy != -1 ? this->divideBy : nvalues - 1);
  // applying a not standard normalization (but used by grass)
  for(int i = 0; i < this->height; i++)
    for(int j = 0; j < this->width; j++){
      netFlow[i][j] /= denominator;
    }

  TIFF *output = openFile("out." + this->filename, this->width, this->height);

  double write_line[this->width];
  for(int i = 0; i < this->height; i++){
    for(int j = 0; j < this->width; j++){
      write_line[j] = netFlow[i][j];
    }
    TIFFWriteScanline(output, write_line, i);
  }

  TIFFClose(output);
  TIFFClose(input);
}
