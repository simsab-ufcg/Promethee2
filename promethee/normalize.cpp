#include "normalize.h"
Matrix Normalizer::normalize(Matrix matrix, MaskMatrix validPixels){
  int nlines = matrix.size();
  int ncolumns = matrix[0].size();
  int posx_min = -1, posy_min = -1;
  int posx_max = -1, posy_max = -1;

  // getting min and max value in the given matrix (to be used in normalization)
  for(int i = 0; i < nlines; i++)
    for(int j = 0; j < ncolumns; j++){
      if(validPixels[i][j]){
        
        if(posx_min == -1){
          posx_min = i;
          posy_min = j;
        } else if(matrix[posx_min][posy_min] > matrix[i][j]){
          posx_min = i;
          posy_min = j;
        }
        
        if(posx_max == -1){
          posx_max = i;
          posy_max = j;
        } else if(matrix[posx_max][posy_max] < matrix[i][j]){
          posx_max = i;
          posy_max = j;
        }
      }
    }

  ldouble max_value = matrix[posx_max][posy_max];
  ldouble min_value = matrix[posx_min][posy_min];
  Matrix normalized = matrix;

  // generating matrix of normalized values
  for(int i = 0; i < nlines; i++)
    for(int j = 0; j < ncolumns; j++)
      if(validPixels[i][j])
        normalized[i][j] = (normalized[i][j] - min_value) / (max_value - min_value);
      else
        normalized[i][j] = -1;
  return normalized;
}
