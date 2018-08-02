#include "normalize.h"
Matrix Normalizer::normalize(Matrix matrix){
  int nlines = matrix.size();
  int ncolumns = matrix[0].size();
  int posx_min = 0, posy_min = 0;
  int posx_max = 0, posy_max = 0;
  for(int i = 0; i < nlines; i++)
    for(int j = 0; j < ncolumns; j++){
      if(matrix[posx_min][posy_min] > matrix[i][j]){
        posx_min = i;
        posx_min = j;
      }
      if(matrix[posx_max][posy_max] < matrix[i][j]){
        posx_max = i;
        posy_max = j;
      }
    }
  ldouble max_value = matrix[posx_max][posy_max];
  ldouble min_value = matrix[posx_min][posy_min];
  Matrix normalized = matrix;
  for(int i = 0; i < nlines; i++)
    for(int j = 0; j < ncolumns; j++)
      normalized[i][j] = (normalized[i][j] - min_value) / (max_value - min_value);
  return normalized;
}
