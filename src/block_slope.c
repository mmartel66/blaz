
#include<stdlib.h>
#include <math.h>
#include<float.h>
#include<blaz.h>


double block_slope(double *matrix, double *result_matrix) {
  int i, j, nb_elts;
  double inf_slope, sup_slope, mean_slope;

  inf_slope = DBL_MAX;
  sup_slope = DBL_MIN;
  nb_elts = 0;
  mean_slope = 0.0;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      if (inf_slope>matrix[POS(j, i, BLOCK_SIZE)]) inf_slope = matrix[POS(j, i, BLOCK_SIZE)];
      if (sup_slope<matrix[POS(j, i, BLOCK_SIZE)]) sup_slope = matrix[POS(j, i, BLOCK_SIZE)];
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
