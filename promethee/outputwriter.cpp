#include "outputwriter.h"

// This method has many parameters, TODO
void showMatrix(string path, int nlines, int ncolumns, int start_line, int start_column, int precision, Matrix matrix, MaskMatrix mask, int grow){
  ofstream out(path);
  out << fixed << setprecision(precision);
  for(int line = start_line; line < nlines + grow; line++)
    for(int column = start_column; column < ncolumns + grow; column++){
      if(line < nlines && column < ncolumns){
        if(!mask[line][column])
          out << "nan";
        else
          out << matrix[line][column];
      } else
        out << "nan";
      out << " \n"[column == ncolumns + grow - 1];
    } 
}
void OutputWriter::write(string path, PrometheeResult result){
  // netFlow, positiveFlow, negativeFlow, mask
  int nlines = result.positiveFlow.size();
  int ncolumns = result.positiveFlow[0].size();

  const int start_line = 0;
  const int start_column = 0;
  const int precision = 14;

  showMatrix(path + "netflow.txt", nlines, ncolumns, start_line, start_column, precision, result.netFlow, result.validPixels, 0);
  showMatrix(path + "positiveflow.txt", nlines, ncolumns, start_line, start_column, precision, result.positiveFlow, result.validPixels, 0);
  showMatrix(path + "negativeflow.txt", nlines, ncolumns, start_line, start_column, precision, result.negativeFlow, result.validPixels, 0);
  showMatrix(path + "normalizedflow.txt", nlines, ncolumns, start_line, start_column, precision, result.normalizedFlow, result.validPixels, 0);

}
