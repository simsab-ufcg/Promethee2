#include "promethee_vanilla.h"
#include "../inputreader.h"
#include "../outputwriter.h"

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


void PrometheeVanilla::process(){

	Data data = this->readData();

	int ncriterias = data.matrices.size();
	int nlines = data.matrices[0].size();
	int ncolumns = data.matrices[0][0].size();

	MaskMatrix validPixels = data.getMaskMatrix();

	Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
	Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
	Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));

	int studyArea = 0;

  	for(int criteria = 0; criteria < ncriterias; criteria++){

		Matrix matrix = data.getCriteriaMatrix(criteria);
		ldouble weight = data.getCriteriaWeight(criteria);
		PrometheeFunctionAdapter* function = data.getFunction(criteria);
		ComparisonFunction* comparison = (*function).getVanilla();
		bool isMax = data.getIsMax(criteria);
		vector<ldouble> values;
		for(int line = 0; line < nlines; line++){
			cerr << (100.0 * (criteria * nlines + line + 1))/(ncriterias * nlines) << "%"<< endl;
			for(int column = 0; column < ncolumns; column++){
				if(!validPixels[line][column]) continue;
				
				studyArea += 1;
				
				for(register int line2 = 0; line2 < nlines; line2++){
					for(register int column2 = 0; column2 < ncolumns; column2++){
						if(validPixels[line2][column2] && 
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

	int denominator = (this->divideBy != -1 ? this->divideBy : studyArea);
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
	result.validPixels = validPixels;

	// normalizing results
	result.normalizedFlow = Normalizer().normalize(netFlow, validPixels);
	
	OutputWriter outputWriter = OutputWriter();
  	outputWriter.write(this->pathToOutput, result);

};