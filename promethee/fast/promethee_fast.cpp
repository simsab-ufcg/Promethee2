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

  // the remaining args are function args
  vector<ldouble> params;
  	for(int i = 2; i < args.size(); i++)
      params.push_back(atof(args[i].c_str()));

  if(type == "linear"){
    this->p = params[0];
    this->q = 0;
  }else{
    this->p = params[0];
    this->q = params[1];
  }
	std::cout << "End of Promethee Initialization" << std::endl;
}

void PrometheeFast::findFirst(int &beginLine, int &beginColunm, ldouble const& firstValue){
	
	bool found = false;
	int atualLine = this->start - 1;

	while(!found && atualLine >= 0){
		int ini = 0, fim = this->width - 1;
		while(ini <= fim){
			int m = (ini + fim) / 2;
			if(image.readPixel(atualLine, m) + this->p < firstValue){
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

void PrometheeFast::findLast(int &beginLine, int &beginColunm, ldouble const& firstValue){
	
	bool found = false;
	int atualLine = this->start;

	while(!found && atualLine < this->height){
		int ini = 0, fim = this->width - 1;
		while(ini <= fim){
			int m = (ini + fim) / 2;
			if(firstValue < image.readPixel(atualLine, m) - this->q){
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

void PrometheeFast::process() {

	// Open input and get fields need to create similar file for output
	std::cout << "Open file..." << std::endl;
	image = BufferManager(filename, 1024);
	std::cout << "File opened." << std::endl;


	this->height = image.getHeight();
	this->width = image.getWidth();

	// Define correct start/end
	this->start = max(0, min(this->start, this->height));
	this->end = max(this->start, min(this->end, this->height));

	// Creating initial -p and q
	ldouble totalSumMinus = 0, totalSumPlus = 0;
	int beginLine = this->start, beginColunm = 0;
	int lastLine = this->start, lastColunm = 0;
	
	ldouble startValue = image.read(0, 0);

	findFirst(beginLine, beginColunm, startValue);
	findLast(lastLine, lastColunm, startValue);

	int begin2Line = beginLine, begin2Colunm = beginColunm;
	int last2Line = lastLine, last2Colunm = lastColunm;
	int qtdMinus = 0;
	int qtdPlus = 0;

	// Setup output files
	string interval = to_string(this->start) + "-" + to_string(this->end);
	TIFF *output = TIFFOpen(("out." + interval + "." + this->filename).c_str(), "w8m");
    TIFFSetField(output, TIFFTAG_IMAGEWIDTH     , this->width); 
    TIFFSetField(output, TIFFTAG_IMAGELENGTH    , end - start);
    TIFFSetField(output, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(output, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(output, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(output, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(output, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(output, TIFFTAG_ROWSPERSTRIP   , min(SIZESTRIP, end - start));
    TIFFSetField(output, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(output, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(output, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(output, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

	// Pointer to buffers
	int minusQPointer = begin2Line;
	int minusPPointer = beginLine;
	int plusQPointer = last2Line;
	int plusPPointer = lastLine;

	// Creating buffers
	ldouble output_line[this->width];

	std::cout << "Prepare to run" << std::endl;
	// Start 4-pointer strategy
	for(int i = this->start; i < this->end; i++){
		for(int j = 0; j < this->width; j++){

			// Go foward with -q pointer
			if(minusQPointer < this->height){ 
				while(image.read(minusQPointer, begin2Colunm) + this->q < image.read(i, j)){
					totalSumMinus += image.read(minusQPointer, begin2Colunm);
					qtdMinus++;
					begin2Colunm++;
					if(begin2Colunm == this->width){
						begin2Colunm = 0;
						minusQPointer++;
						if(minusQPointer == this->height)break;
					}
				}
			}

			// Go foward with -p pointer
			if(minusPPointer < this->height){ 
				while(image.read(minusPPointer, beginColunm) + this->p < image.read(i, j)){
					totalSumMinus -= image.read(minusPPointer, beginColunm);
					qtdMinus--;
					beginColunm++;
					if(beginColunm == this->width){
						beginColunm = 0;
						minusPPointer++;
						if(minusPPointer == this->height)break;
					}
				}
			}

			// Go foward with q pointer
			if(plusQPointer < this->height){ 
				while(image.read(plusQPointer, last2Colunm) - this->q < image.read(i, j)){
					totalSumPlus -= image.read(plusQPointer, last2Colunm);
					qtdPlus--;
					last2Colunm++;
					if(last2Colunm == this->width){
						last2Colunm = 0;
						plusQPointer++;
						if(plusQPointer == this->height)break;
					}
				}
			}

			// Go foward with p pointer
			if(plusPPointer < this->height){ 
				while(image.read(plusPPointer, lastColunm) - this->p < image.read(i, j)){
					totalSumPlus += image.read(plusPPointer, lastColunm);
					qtdPlus++;
					lastColunm++;
					if(lastColunm == this->width){
						lastColunm = 0;
						plusPPointer++;
						if(plusPPointer == this->height)break;
					}
				}
			}
			
			// After update all pointer, time to calculate flow
			ldouble plusFlow = ((minusPPointer * this->width) + (beginColunm));
			plusFlow += ((abs(totalSumMinus - (qtdMinus * image.read(i, j))) - (this->q * (ldouble)qtdMinus)) / ((ldouble)this->p - this->q)); 
			
			ldouble minusFlow = (this->height - plusPPointer) * (this->width) - (lastColunm + (plusPPointer != this->height));
			minusFlow += ((abs(totalSumPlus - (qtdPlus * image.read(i, j))) - (this->q * (ldouble)qtdPlus)) / ((ldouble)this->p - this->q));
 
			output_line[j] = ((plusFlow - minusFlow) * this->weight * (this->isMax ? 1.0 : -1.0)) / ((this->height * this->width) - 1.0);
		}
		TIFFWriteScanline(output, output_line, i - this->start);
	}

	std::cout << "Done promethee" << std::endl;	

	TIFFClose(output);
	image.close();
}
