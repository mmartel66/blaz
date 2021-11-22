/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : io.c
 * @Role ........ : main functions for compression and
                    uncompression
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <blaz.h>


Blaz_Compressed_Matrix *blaz_compress(Blaz_Matrix *matrix) {
  int i, j, k, l;
  double *delta_block, *slope_block;
  Blaz_Compressed_Matrix *result_matrix;

  k = l = 0;
  delta_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  result_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));
  result_matrix->block_first_elts = (double*)blaz_malloc(matrix->width * matrix->height * sizeof(double) / (BLOCK_SIZE * BLOCK_SIZE));
  result_matrix->block_mean_slope = (double*)blaz_malloc(matrix->width * matrix->height * sizeof(double) / (BLOCK_SIZE * BLOCK_SIZE));
  result_matrix->compressed_values = (s_8*)blaz_malloc(matrix->width * matrix->height * COMPRESSED_VECTOR_SIZE * sizeof(s_8) / (BLOCK_SIZE * BLOCK_SIZE));

  result_matrix->width = matrix->width;
  result_matrix->height = matrix->height;

  for(i=0; i<matrix->height; i+=BLOCK_SIZE) {
    for(j=0; j<matrix->width; j+=BLOCK_SIZE) {
      result_matrix->block_first_elts[k] = block_delta(matrix->matrix, delta_block, j, i, matrix->width);
      result_matrix->block_mean_slope[k] = block_slope(delta_block, slope_block);
      dct(slope_block, result_matrix->compressed_values, l);
      k++;
      l += COMPRESSED_VECTOR_SIZE;
    }
  }

  free(delta_block);
  free(slope_block);

  return result_matrix;
}


Blaz_Matrix *blaz_uncompress(Blaz_Compressed_Matrix *compressed_matrix) {
  int i, j, k, l;
  double *delta_block, *slope_block;
  Blaz_Matrix *matrix;

  matrix = (Blaz_Matrix*)blaz_malloc(sizeof(Blaz_Matrix));
  matrix->width = compressed_matrix->width;
  matrix->height = compressed_matrix->height;

  k = l = 0;
  matrix->matrix = (double*)blaz_malloc(matrix->width * matrix->height * sizeof(double));

  delta_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  for(i=0; i<matrix->height; i+=BLOCK_SIZE) {
    for(j=0; j<matrix->width; j+=BLOCK_SIZE) {
      idct(slope_block, compressed_matrix->compressed_values, l);
      block_unslope(slope_block, delta_block, compressed_matrix->block_mean_slope[k]);
      block_undelta(delta_block, matrix->matrix, compressed_matrix->block_first_elts[k], j, i, matrix->width);
      k++;
      l += COMPRESSED_VECTOR_SIZE;
    }
  }

  free(delta_block);
  free(slope_block);

  return matrix;
}
