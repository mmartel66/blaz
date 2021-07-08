/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : block_delta.c
 * @Role ........ : normalization during the compression.
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <blaz.h>

double block_delta(double *matrix, double *result_matrix, int block_x, int block_y, int matrix_width) {
  int i, j;

  result_matrix[POS(0, 0, BLOCK_SIZE)] = 0.0;
  for(i=1; i<BLOCK_SIZE; i++) {
    result_matrix[POS(i, 0, BLOCK_SIZE)] = matrix[POS(block_x + i, block_y, matrix_width)] - matrix[POS(block_x + i - 1, block_y, matrix_width)];
    result_matrix[POS(0, i, BLOCK_SIZE)] = matrix[POS(block_x, block_y + i, matrix_width)] - matrix[POS(block_x, block_y + i - 1, matrix_width)];
  }

  for(i=1; i<BLOCK_SIZE; i++) {
    for(j=1; j<BLOCK_SIZE; j++) {
      result_matrix[POS(j, i, BLOCK_SIZE)] = matrix[POS(block_x + j, block_y + i, matrix_width)]
                                           - ( matrix[POS(block_x + j - 1, block_y + i, matrix_width)]
                                             + matrix[POS(block_x + j, block_y + i - 1, matrix_width)] ) / 2.0;
    }
  }
  return matrix[POS(block_x, block_y, matrix_width)];
}


void block_undelta(double *matrix, double *result_matrix, double delta_block_first_elt, int block_x, int block_y, int matrix_width) {
  int i, j;

  result_matrix[POS(block_x, block_y, matrix_width)] = delta_block_first_elt;

  for(i=1; i<BLOCK_SIZE; i++) {
    result_matrix[POS(block_x + i, block_y, matrix_width)] = result_matrix[POS(block_x + i - 1, block_y, matrix_width)]
                                                           + matrix[POS(i, 0, BLOCK_SIZE)];
    result_matrix[POS(block_x, block_y + i, matrix_width)] = result_matrix[POS(block_x, block_y + i - 1, matrix_width)]
                                                           + matrix[POS(0, i, BLOCK_SIZE)];
  }

  for(i=1; i<BLOCK_SIZE; i++) {
    for(j=1; j<BLOCK_SIZE; j++) {
      result_matrix[POS(block_x + j,block_y + i,matrix_width)] = matrix[POS(j,i, BLOCK_SIZE)]
                                                               + (result_matrix[POS(block_x + j - 1, block_y + i,matrix_width)] +
                                                                  result_matrix[POS(block_x + j, block_y + i - 1,matrix_width)]) / 2.0;
    }
  }
}
