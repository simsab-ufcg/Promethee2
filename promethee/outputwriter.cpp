#include "outputwriter.h"
void OutputWriter::write(string path, vector<Matrix> result){
  // netFlow, positiveFlow, negativeFlow
  int nline = result[0].size();
  int ncolumn = result[0][0].size();

  const int start_line = 1;
  const int start_column = 1;
  const int precision = 7;

  ofstream netOut(path + "netflow.txt");
  netOut << fixed << setprecision(precision);
  for(int line = start_line; line < nline; line++)
    for(int column = start_column; column < ncolumn; column++)
      netOut << result[0][line][ncolumn] << " \n"[column == ncolumn - 1];

  ofstream positiveOut(path + "positiveflow.txt");
  positiveOut << fixed << setprecision(precision);
  for(int line = start_line; line < nline; line++)
    for(int column = start_column; column < ncolumn; column++)
      positiveOut << result[1][line][column] << " \n"[column == ncolumn - 1];

  ofstream negativeOut(path + "negativeflow.txt");
  negativeOut << fixed << setprecision(precision);
  for(int line = start_line; line < nline; line++)
    for(int column = start_column; column < ncolumn; column++)
      negativeOut << result[2][line][column] << " \n"[column == ncolumn - 1];
}
