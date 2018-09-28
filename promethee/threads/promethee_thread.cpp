#include "promethee_thread.h"
#include "../promethee_function_adapter.h"
#include "../normalize.h"
#include <thread>
#include <iostream>
#include "../inputreader.h"
#include "../outputwriter.h"
#include "../parse_directory.h"

Data PrometheeThread::readData(){
  InputReader inputReader = InputReader();
  Data data = Data();
  for(int i = 0; i < this->inputFiles.size(); i++){
    Matrix nmatrix = inputReader.readMatrix(this->inputFiles[i]);
    MatrixMetaData metaData = inputReader.readMetaData(this->metaFiles[i], false);
    data.addCriteria(nmatrix, metaData);
  }
  data.normalizeWeights();
  return data;
}

void PrometheeThread::init(vector<string> args, int divideBy){
  this->divideBy = divideBy;
  parseInputAndMeta(args, this->inputFiles, this->metaFiles, this->pathToOutput);
}


void worker(argsThread args){

    Matrix* matrix = args.input;
    Matrix* positive = args.positiveOut;
    Matrix* negative = args.negativeOut;
    MaskMatrix* validPixels = args.validPixels;
    PrometheeFunctionAdapter* function = args.function;
    ldouble weight = args.weight;
    bool isMax = args.isMax;

    int nlines = (*matrix).size();
    int ncolumns = (*matrix)[0].size();

    vector<ldouble> values;

    for(int line = 0; line < nlines; line++) {
      for(int column = 0; column < ncolumns; column++) {
        if((*validPixels)[line][column]) {
          values.push_back((*matrix)[line][column]);
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

    for(int line = 0; line < nlines; line++){
      for(int column = 0; column < ncolumns; column++){
        if((*validPixels)[line][column]){

          if(isMax){
            (*positive)[line][column] += (*function).getPositiveDelta(values, (*matrix)[line][column], cummulative, weight);
            (*negative)[line][column] += (*function).getNegativeDelta(values, (*matrix)[line][column], cummulative, weight);
          } else {
            (*negative)[line][column] += (*function).getPositiveDelta(values, (*matrix)[line][column], cummulative, weight);
            (*positive)[line][column] += (*function).getNegativeDelta(values, (*matrix)[line][column], cummulative, weight);
          }

        }
      }
    }

}

void PrometheeThread::process(){

    Data data = this->readData();

    int ncriterias = data.matrices.size();
    int nlines = data.matrices[0].size();
    int ncolumns = data.matrices[0][0].size();

    MaskMatrix validPixels = data.getMaskMatrix();

    int validValues = 0;

    for(int line = 0; line < nlines; line++)
        for(int column = 0; column < ncolumns; column++)
            if(validPixels[line][column])
                validValues += 1;

    vector<Matrix> positive = vector<Matrix>(ncriterias, Matrix(nlines, MatrixLine(ncolumns, 0.0)));
    vector<Matrix> negative = vector<Matrix>(ncriterias, Matrix(nlines, MatrixLine(ncolumns, 0.0)));

    thread threads[ncriterias];

    vector<Matrix> matrices;
    vector<ldouble> weights;
    vector<PrometheeFunctionAdapter*> functions;
    vector<bool> isMax;
    for(int criteria = 0; criteria < ncriterias; criteria++){
        matrices.push_back(data.getCriteriaMatrix(criteria));
        weights.push_back(data.getCriteriaWeight(criteria));
        functions.push_back(data.getFunction(criteria));
        isMax.push_back(data.getIsMax(criteria));
    }

    for(int criteria = 0; criteria < ncriterias; criteria++){

        argsThread myArgs;
        myArgs.input = &matrices[criteria];
        myArgs.weight = weights[criteria];
        myArgs.validPixels = &validPixels;
        myArgs.positiveOut = &positive[criteria];
        myArgs.negativeOut = &negative[criteria];
        myArgs.function = functions[criteria];
        myArgs.isMax = isMax[criteria];

        threads[criteria] = thread(worker, myArgs);
    }

    for(int criteria = 0; criteria < ncriterias; criteria++)
        if(threads[criteria].joinable())
            threads[criteria].join();

    Matrix positiveFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
    Matrix negativeFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));
    Matrix netFlow = Matrix(nlines, MatrixLine(ncolumns, 0.0));

    for(int criteria = 0; criteria < ncriterias; criteria++){
        for(int line = 0; line < nlines; line++)
            for(int column = 0; column < ncolumns; column++){
                positiveFlow[line][column] += positive[criteria][line][column];
                negativeFlow[line][column] += negative[criteria][line][column];
            }
    }

  int denominator = (this->divideBy == -1 ? validValues - 1 : this->divideBy);

  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++){
      positiveFlow[line][column] /= denominator;
      negativeFlow[line][column] /= denominator;
    }

  // calculating global flow
  for(int line = 0; line < nlines; line++) {
    for(int column = 0; column < ncolumns; column++) {
      netFlow[line][column] = positiveFlow[line][column] - negativeFlow[line][column];
    }
  }

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
    
}