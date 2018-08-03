#include "outputwriter.h"
void showMatrix(string path, int nlines, int ncolumns, int start_line, int start_column, int precision, Matrix matrix){
  ofstream out(path);
  out << fixed << setprecision(precision);
  for(int line = start_line; line < nlines + 2; line++)
    for(int column = start_column; column < ncolumns + 2; column++){
      if(line < nlines && column < ncolumns){
        if(matrix[line][column] < 0)
          out << "nan";
        else
          out << matrix[line][column];
      } else
        out << "nan";
      out << " \n"[column == ncolumns + 1];
    } 
}
void OutputWriter::write(string path, PrometheeResult result){
  // netFlow, positiveFlow, negativeFlow, mask
  int nlines = result.positiveFlow.size();
  int ncolumns = result.positiveFlow[0].size();

  const int start_line = 1;
  const int start_column = 1;
  const int precision = 14;

  showMatrix(path + "netflow.txt", nlines, ncolumns, start_line, start_column, precision, result.netFlow);
  showMatrix(path + "positiveflow.txt", nlines, ncolumns, start_line, start_column, precision, result.positiveFlow);
  showMatrix(path + "negativeflow.txt", nlines, ncolumns, start_line, start_column, precision, result.negativeFlow);
  showMatrix(path + "normalizedflow.txt", nlines, ncolumns, start_line, start_column, precision, result.normalizedFlow);

}
