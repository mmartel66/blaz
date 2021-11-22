/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : block_slope.c
 * @Role ........ : during compression, replace delta
                    by a slope encoded in a 8 bits integer
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 22/11/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include<stdlib.h>
#include <math.h>
#include<float.h>
#include<blaz.h>


double block_slope(double *matrix, double *result_matrix) {
  int i, j, nb_elts;
  double mean_slope;

  nb_elts = 0;
  mean_slope = 0.0;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      mean_slope += fabs(matrix[POS(j, i, BLOCK_SIZE)]);
      if (matrix[POS(j, i, BLOCK_SIZE)] != 0.0) nb_elts++;
    }
  }

  mean_slope /= nb_elts;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      result_matrix[POS(j, i, BLOCK_SIZE)] = matrix[POS(j, i, BLOCK_SIZE)] / mean_slope;
    }
  }

  return mean_slope;
}

void block_unslope(double *matrix, double *result_matrix, double mean_slope) {
  int i, j;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      result_matrix[POS(j, i, BLOCK_SIZE)] = mean_slope * matrix[POS(j, i, BLOCK_SIZE)];
    }
  }
}
