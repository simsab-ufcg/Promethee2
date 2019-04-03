#include "promethee_fast.h"
#include "../normalize.h"
#include <iostream>
#include "../inputreader.h"
#include "../outputwriter.h"
#include "../plibtiff.h"
#include "../parse_directory.h"
#include "../parse_args.h"

void PrometheeFast::init(vector<string> args, int divideBy){
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

	string chunkSize = getCmdOption(args, "-size");
	if(!chunkSize.size()){
		cerr << "Arguments is missing!" << endl;
		exit(1);
	}

	string start = getCmdOption(args, "-start");
	if(!start.size()){
		this->start = -1;
	}else{
		this->start = atoi(start.c_str());
	}

	string end = getCmdOption(args, "-end");
	if(!end.size()){
		this->end = 1e9;
	}else{
		this->end = atoi(end.c_str());
	}
  if(type == "linear"){
    this->p = params[0];
    this->q = 0;
  }else{
    this->p = params[0];
    this->q = params[1];
  }

	this->es = ExternalSort(this->filename, atoi(chunkSize.c_str()) * 512 * 1024);

	es.sort();

}

void PrometheeFast::findFirst(PixelReader &pr, int &beginLine, int &beginColunm, ldouble const& firstValue){
	
	bool found = false;
	int atualLine = this->start - 1;

	while(!found && atualLine >= 0){
		TIFFReadScanline(this->input, pr.buffer, atualLine);
		int ini = 0, fim = this->width - 1;
		while(ini <= fim){
			int m = (ini + fim) / 2;
			if(pr.readPixel(m) + this->p < firstValue){
				ini = m + 1;
				found = true;
			}else{
				beginLine = atualLine;
				beginColunm = m;
				fim = m - 1;
			}
		}
		atualLine--;
	}

}

void PrometheeFast::findLast(PixelReader &pr, int &beginLine, int &beginColunm, ldouble const& firstValue){
	
	bool found = false;
	int atualLine = this->start;

	while(!found && atualLine < this->height){
		TIFFReadScanline(this->input, pr.buffer, atualLine);
		int ini = 0, fim = this->width - 1;
		while(ini <= fim){
			int m = (ini + fim) / 2;
			if(firstValue < pr.readPixel(m) - this->q){
				fim = m - 1;
				found = true;
			}else{
				beginLine = atualLine;
				beginColunm = m;
				ini = m + 1;
			}
		}
		atualLine++;
	}

}

void PrometheeFast::fillBuffer(ldouble buffer[], PixelReader &pr, int line){
	TIFFReadScanline(this->input, pr.buffer, line);
	for(register int i = 0; i < this->width; i++)
		buffer[i] = pr.readPixel(i);
}

void PrometheeFast::process() {

	// Open input and get fields need to create similar file for output
	this->input = TIFFOpen(this->filename.c_str(), "rm");
  TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &this->width);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &this->height);
	TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &this->sampleFormat);
	TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &this->samplePerPixel);

	// Define correct start/end
	this->start = max(0, min(this->start, this->height));
  this->end = max(this->start, min(this->end, this->height));

  // Create PixelReader Interface
  unsigned short byte_size = TIFFScanlineSize(input) / this->width;
  tdata_t line = _TIFFmalloc(TIFFScanlineSize(input));
  PixelReader pr = PixelReader(this->sampleFormat, byte_size, line);
	TIFFReadScanline(input, line, this->start);

	// Creating initial -p and q
	ldouble totalSumMinus = 0, totalSumPlus = 0;
	int beginLine = this->start, beginColunm = 0;
	int lastLine = this->start, lastColunm = 0;
	
	ldouble startValue = pr.readPixel(0);

	findFirst(pr, beginLine, beginColunm, startValue);
	findLast(pr, lastLine, lastColunm, startValue);

	int begin2Line = beginLine, begin2Colunm = beginColunm;
	int last2Line = lastLine, last2Colunm = lastColunm;
	int qtdMinus = 0;
	int qtdPlus = 0;


	// Setup output files
	string interval = to_string(this->start) + "-" + to_string(this->end);
	TIFF *output = openFile("out." + interval + "." + this->filename, this->width, this->end - this->start);

	// Pointer to buffers
	int minusQPointer = begin2Line;
	int minusPPointer = beginLine;
	int plusQPointer = last2Line;
	int plusPPointer = lastLine;

	// Creating buffers
	ldouble output_line[this->width];
	ldouble bufferMinusQ[this->width];
	ldouble bufferMinusP[this->width];
	ldouble bufferPlusQ[this->width];
	ldouble bufferPlusP[this->width];
	ldouble bufferInput[this->width];

	fillBuffer(bufferMinusQ, pr, minusQPointer);
	fillBuffer(bufferMinusP, pr, minusPPointer);
	fillBuffer(bufferPlusQ, pr, plusQPointer);
	fillBuffer(bufferPlusP, pr, plusPPointer);

	// Start 4-pointer strategy
	for(int i = this->start; i < this->end; i++){
		fillBuffer(bufferInput, pr, i);
		for(int j = 0; j < this->width; j++){

			// Go foward with -q pointer
			if(minusQPointer < this->height){ 
				while(bufferMinusQ[begin2Colunm] + this->q < bufferInput[j]){
					totalSumMinus += bufferMinusQ[begin2Colunm];
					qtdMinus++;
					begin2Colunm++;
					if(begin2Colunm == this->width){
						begin2Colunm = 0;
						minusQPointer++;
						if(minusQPointer == this->height)break;
						fillBuffer(bufferMinusQ, pr, minusQPointer);
					}
				}
			}

			// Go foward with -p pointer
			if(minusPPointer < this->height){ 
				while(bufferMinusP[beginColunm] + this->p < bufferInput[j]){
					totalSumMinus -= bufferMinusP[beginColunm];
					qtdMinus--;
					beginColunm++;
					if(beginColunm == this->width){
						beginColunm = 0;
						minusPPointer++;
						if(minusPPointer == this->height)break;
						fillBuffer(bufferMinusP, pr, minusPPointer);
					}
				}
			}

			// Go foward with q pointer
			if(plusQPointer < this->height){ 
				while(bufferPlusQ[last2Colunm] - this->q < bufferInput[j]){
					totalSumPlus -= bufferPlusQ[last2Colunm];
					qtdPlus--;
					last2Colunm++;
					if(last2Colunm == this->width){
						last2Colunm = 0;
						plusQPointer++;
						if(plusQPointer == this->height)break;
						fillBuffer(bufferPlusQ, pr, plusQPointer);
					}
				}
			}

			// Go foward with p pointer
			if(plusPPointer < this->height){ 
				while(bufferPlusP[lastColunm] - this->p < bufferInput[j]){
					totalSumPlus += bufferPlusP[lastColunm];
					qtdPlus++;
					lastColunm++;
					if(lastColunm == this->width){
						lastColunm = 0;
						plusPPointer++;
						if(plusPPointer == this->height)break;
						fillBuffer(bufferPlusP, pr, plusPPointer);
					}
				}
			}
			
			// After update all pointer, time to calculate flow
			ldouble plusFlow = ((minusPPointer * this->width) + (beginColunm));
			plusFlow += ((abs(totalSumMinus - (qtdMinus * bufferInput[j])) - (this->q * (ldouble)qtdMinus)) / ((ldouble)this->p - this->q)); 
			
			ldouble minusFlow = (this->height - plusPPointer) * (this->width) - (lastColunm + (plusPPointer != this->height));
			minusFlow += ((abs(totalSumPlus - (qtdPlus * bufferInput[j])) - (this->q * (ldouble)qtdPlus)) / ((ldouble)this->p - this->q));

			
			output_line[j] = ((plusFlow - minusFlow) * this->weight * (this->isMax ? 1.0 : -1.0)) / ((this->height * this->width) - 1.0);
		}
		TIFFWriteScanline(output, output_line, i - this->start);
	}
	TIFFClose(output);
	TIFFClose(input);

	es.path = "out." + interval + "." + this->filename;
	es.reverse();
}
