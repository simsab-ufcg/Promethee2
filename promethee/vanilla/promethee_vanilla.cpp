#include "promethee_vanilla.h"
#include "../inputreader.h"
#include "../outputwriter.h"
#include "../parse_directory.h"
#include <algorithm>
#include "../parse_args.h"

Data PrometheeVanilla::readData(){
  InputReader inputReader = InputReader();
  Data data = Data();
  for(int i = 0; i < this->inputFiles.size(); i++){
    Matrix nmatrix = inputReader.readMatrix(this->inputFiles[i]);
    MatrixMetaData metaData = inputReader.readMetaData(this->metaFiles[i], true);
    data.addCriteria(nmatrix, metaData);
  }
  data.normalizeWeights();
  return data;
}

void PrometheeVanilla::init(vector<string> args, int divideBy){
	this->divideBy = divideBy;
	Matrix nmatrix;
	TIFF *input = TIFFOpen(args[0].c_str(), "rm");
  	int width, height, sampleFormat, samplePerPixel;
	TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(input, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(input, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
	TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
  	//cout << args.size() << endl;
	unsigned short byte_size = TIFFScanlineSize(input) / width;
  	tdata_t line = _TIFFmalloc(TIFFScanlineSize(input));
  	PixelReader pr = PixelReader(sampleFormat, byte_size, line);	
	//cout << args.size() << endl;
	for(int i = 0; i < height; i++){
		MatrixLine row;
		TIFFReadScanline(input, line, i);
		for(int j = 0; j < width; j++){
			row.push_back(pr.readPixel(j));
		}
		nmatrix.push_back(row);
	}
	MatrixMetaData metaData;
	metaData.weight = atof(args[1].c_str());
	metaData.isMax = hasFlag(args, "-ismax");
	metaData.name = args[2];
	vector <double> params;
	for(int i = 3; i < args.size(); i++){
		params.push_back(atof(args[i].c_str()));
	}
	//cout << "bla" << endl;
	metaData.function = new PrometheeFunctionAdapter(false, params, args[2]);
	this->data = Data();
	data.addCriteria(nmatrix, metaData);
	//cout << "oi" << endl;
	TIFFClose(input);
	//cout << "ola" << endl;
}

void PrometheeVanilla::process(){

	Data data = this->data;

	int ncriterias = data.matrices.size();
	int nlines = data.matrices[0].size();
	int ncolumns = data.matrices[0][0].size();

	Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
	Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
	Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));

	int studyArea = 0;

	for(int line = 0; line < nlines; line++)
		for(int column = 0; column < ncolumns; column++){
			if(!isnan(data.matrices[0][line][column])){
				studyArea++;
			}
		}

  	for(int criteria = 0; criteria < ncriterias; criteria++){

		Matrix matrix = data.getCriteriaMatrix(criteria);
		ldouble weight = data.getCriteriaWeight(criteria);
		PrometheeFunctionAdapter* function = data.getFunction(criteria);
		ComparisonFunction* comparison = (*function).getVanilla();
		bool isMax = data.getIsMax(criteria);
		vector<ldouble> values;
		for(int line = 0; line < nlines; line++){
			for(int column = 0; column < ncolumns; column++){
				if(isnan(data.matrices[0][line][column])) continue;
				
				for(register int line2 = 0; line2 < nlines; line2++){
					for(register int column2 = 0; column2 < ncolumns; column2++){
						if(!isnan(data.matrices[0][line][column]) && 
						(line != line2 || column != column2)){

							if(isMax){
								positiveFlow[line][column] += weight * (*comparison)(matrix[line][column], matrix[line2][column2]);
								negativeFlow[line][column] += weight * (*comparison)(matrix[line2][column2], matrix[line][column]);
							} else {
								positiveFlow[line][column] += weight * (*comparison)(matrix[line2][column2], matrix[line][column]);
								negativeFlow[line][column] += weight * (*comparison)(matrix[line][column], matrix[line2][column2]);
							}
						}
					}
				}
			}
		}

  	}

	int denominator = (this->divideBy != -1 ? this->divideBy : studyArea - 1);
	// applying a not standard normalization (but used by grass)
	for(int line = 0; line < nlines; line++)
	for(int column = 0; column < ncolumns; column++){
		positiveFlow[line][column] /= denominator;
		negativeFlow[line][column] /= denominator;
	}

	// calculating global flow
	for(int line = 0; line < nlines; line++)
	for(int column = 0; column < ncolumns; column++)
		netFlow[line][column] = positiveFlow[line][column] - negativeFlow[line][column];

	// generating results 
	PrometheeResult result = PrometheeResult();
	result.positiveFlow = positiveFlow;
	result.negativeFlow = negativeFlow;
	result.netFlow = netFlow;

	TIFF *output = TIFFOpen("out.promethee.tif", "w8m");
    TIFFSetField(output, TIFFTAG_IMAGEWIDTH     , ncolumns); 
    TIFFSetField(output, TIFFTAG_IMAGELENGTH    , nlines);
    TIFFSetField(output, TIFFTAG_BITSPERSAMPLE  , 64);
    TIFFSetField(output, TIFFTAG_SAMPLEFORMAT   , 3);
    TIFFSetField(output, TIFFTAG_COMPRESSION    , 1);
    TIFFSetField(output, TIFFTAG_PHOTOMETRIC    , 1);
    TIFFSetField(output, TIFFTAG_ORIENTATION    , 1);
    TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(output, TIFFTAG_ROWSPERSTRIP   , 1);
    TIFFSetField(output, TIFFTAG_RESOLUTIONUNIT , 1);
    TIFFSetField(output, TIFFTAG_XRESOLUTION    , 1);
    TIFFSetField(output, TIFFTAG_YRESOLUTION    , 1);
    TIFFSetField(output, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG   );

	ldouble output_line[ncolumns];

	for(int i = 0; i < nlines; i++){
		for(int j = 0; j < ncolumns; j++){
			output_line[j] = netFlow[i][j];
		}
		TIFFWriteScanline(output, output_line, i);
	}

	TIFFClose(output);
};