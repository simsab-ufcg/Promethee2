#include "outputwriter.h"

void showMatrix(string path, int precision, Matrix matrix, MaskMatrix mask){
  
  int nlines = matrix.size();
  int ncolumns = matrix[0].size();

  ofstream out(path);
  out << fixed << setprecision(precision);
  
  for(int line = 0; line < nlines; line++)
    for(int column = 0; column < ncolumns; column++){
      if(!mask[line][column])
        out << "nan";
      else
        out << matrix[line][column];
      out << " \n"[column == ncolumns - 1];
    } 

}
void OutputWriter::write(string path, PrometheeResult result){
  // netFlow, positiveFlow, negativeFlow, mask
  int nlines = result.positiveFlow.size();
  int ncolumns = result.positiveFlow[0].size();

  const int precision = 14;

  showMatrix(path + "netflow.txt", precision, result.netFlow, result.validPixels);
  showMatrix(path + "positiveflow.txt", precision, result.positiveFlow, result.validPixels);
  showMatrix(path + "negativeflow.txt", precision, result.negativeFlow, result.validPixels);
  showMatrix(path + "normalizedflow.txt", precision, result.normalizedFlow, result.validPixels);

}
