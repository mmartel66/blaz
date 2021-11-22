/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : compressed_operations.c
 * @Role ........ : operations on compressed matrices
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 22/11/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <math.h>
#include <blaz.h>


double blaz_get_compressed_matrix_elt(Blaz_Compressed_Matrix *compressed_matrix, int column, int line) {
  int block_x, block_y, offset_x, offset_y, block_number;
  double *slope_block, *delta_block, *uncompressed_block;
  double res;

  block_x = column / BLOCK_SIZE;
  block_y = line / BLOCK_SIZE;
  offset_x = column % BLOCK_SIZE;
  offset_y = line % BLOCK_SIZE;
  block_number = block_y * compressed_matrix->width / BLOCK_SIZE + block_x;

  delta_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  uncompressed_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  idct(slope_block, compressed_matrix->compressed_values, block_number * COMPRESSED_VECTOR_SIZE);
  block_unslope(slope_block, delta_block, compressed_matrix->block_mean_slope[block_number]);
  block_undelta(delta_block, uncompressed_block, compressed_matrix->block_first_elts[block_number], 0, 0, BLOCK_SIZE);

  res = uncompressed_block[POS(offset_x, offset_y, BLOCK_SIZE)];

  free(delta_block);
  free(slope_block);
  free(uncompressed_block);

  return res;
}


void blaz_set_compressed_matrix_elt(Blaz_Compressed_Matrix *compressed_matrix, double value, int column, int line) {
  int block_x, block_y, offset_x, offset_y, block_number;
  double *slope_block, *delta_block, *uncompressed_block;

  block_x = column / BLOCK_SIZE;
  block_y = line / BLOCK_SIZE;
  offset_x = column % BLOCK_SIZE;
  offset_y = line % BLOCK_SIZE;
  block_number = block_y * compressed_matrix->width / BLOCK_SIZE + block_x;

  delta_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  uncompressed_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  idct(slope_block, compressed_matrix->compressed_values, block_number * COMPRESSED_VECTOR_SIZE);
  block_unslope(slope_block, delta_block, compressed_matrix->block_mean_slope[block_number]);
  block_undelta(delta_block, uncompressed_block, compressed_matrix->block_first_elts[block_number], 0, 0, BLOCK_SIZE);

  uncompressed_block[POS(offset_x, offset_y, BLOCK_SIZE)] = value;

  compressed_matrix->block_first_elts[block_number] = block_delta(uncompressed_block, delta_block, 0, 0, BLOCK_SIZE);
  compressed_matrix->block_mean_slope[block_number] = block_slope(delta_block, slope_block);
  dct(slope_block, compressed_matrix->compressed_values, block_number * COMPRESSED_VECTOR_SIZE);

  free(delta_block);
  free(slope_block);
  free(uncompressed_block);
}


Blaz_Compressed_Matrix *blaz_add_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2) {
  int i, j, k;
  double coef_1, coef_2;
  Blaz_Compressed_Matrix *result_matrix;

  result_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));

  result_matrix->width = matrix_1->width;
  result_matrix->height = matrix_1->height;

  result_matrix->block_first_elts = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->block_mean_slope = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->compressed_values = (s_8*)blaz_malloc(result_matrix->width * result_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

  for(i=0; i<result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    result_matrix->block_first_elts[i] = matrix_1->block_first_elts[i] + matrix_2->block_first_elts[i];
    result_matrix->block_mean_slope[i] = matrix_1->block_mean_slope[i] + matrix_2->block_mean_slope[i];
  }

  k = 0;

  for(i=0; i < result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    coef_1 = matrix_1->block_mean_slope[i] / (matrix_1->block_mean_slope[i] + matrix_2->block_mean_slope[i]);
    coef_2 = matrix_2->block_mean_slope[i] / (matrix_1->block_mean_slope[i] + matrix_2->block_mean_slope[i]);
    result_matrix->compressed_values[k] = (s_8)(round((matrix_1->compressed_values[k] * matrix_2->compressed_values[k])
                                                    / (matrix_1->compressed_values[k] + matrix_2->compressed_values[k])));
    coef_1 = coef_1 / matrix_1->compressed_values[k] * result_matrix->compressed_values[k];
    coef_2 = coef_2 / matrix_2->compressed_values[k] * result_matrix->compressed_values[k];
    k++;
    for(j=1; j<COMPRESSED_VECTOR_SIZE; j++) {
      result_matrix->compressed_values[k] = (s_8)(round(coef_1 * (double)(matrix_1->compressed_values)[k])
                                                     + (coef_2 * (double)(matrix_2->compressed_values)[k]));
      k++;
    }
  }

  return result_matrix;
}


Blaz_Compressed_Matrix *blaz_sub_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2) {
  int i, j, k;
  double coef_1, coef_2;
  Blaz_Compressed_Matrix *result_matrix;

  result_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));

  result_matrix->width = matrix_1->width;
  result_matrix->height = matrix_1->height;

  result_matrix->block_first_elts = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->block_mean_slope = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->compressed_values = (s_8*)blaz_malloc(result_matrix->width * result_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

  for(i=0; i<result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    result_matrix->block_first_elts[i] = matrix_1->block_first_elts[i] - matrix_2->block_first_elts[i];
    result_matrix->block_mean_slope[i] = matrix_1->block_mean_slope[i] - matrix_2->block_mean_slope[i];
  }

  k = 0;

  for(i=0; i < result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    coef_1 = matrix_1->block_mean_slope[i] / (matrix_1->block_mean_slope[i] - matrix_2->block_mean_slope[i]);
    coef_2 = -matrix_2->block_mean_slope[i] / (matrix_1->block_mean_slope[i] - matrix_2->block_mean_slope[i]);
    result_matrix->compressed_values[k] = (s_8)(round((matrix_1->compressed_values[k] * matrix_2->compressed_values[k])
                                                    / (matrix_1->compressed_values[k] + matrix_2->compressed_values[k])));
    coef_1 = coef_1 / matrix_1->compressed_values[k] * result_matrix->compressed_values[k];
    coef_2 = coef_2 / matrix_2->compressed_values[k] * result_matrix->compressed_values[k];
    k++;
    for(j=1; j<COMPRESSED_VECTOR_SIZE; j++) {
      result_matrix->compressed_values[k] = (s_8)(round(coef_1 * (double)(matrix_1->compressed_values)[k])
                                                     + (coef_2 * (double)(matrix_2->compressed_values)[k]));
      k++;
    }
  }

  return result_matrix;
}


Blaz_Compressed_Matrix *blaz_mul_cst_compressed(Blaz_Compressed_Matrix *matrix, double cst) {
  int i, j, k;
  Blaz_Compressed_Matrix *result_matrix;

  result_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));

  result_matrix->width = matrix->width;
  result_matrix->height = matrix->height;

  result_matrix->block_first_elts = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->block_mean_slope = (double*)blaz_malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->compressed_values = (s_8*)blaz_malloc(result_matrix->width * result_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

  for(i=0; i<result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    result_matrix->block_first_elts[i] = matrix->block_first_elts[i] * cst;
    result_matrix->block_mean_slope[i] = matrix->block_mean_slope[i] * cst;
  }

  k = 0;

  for(i=0; i < result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    result_matrix->compressed_values[k] = matrix->compressed_values[k];
    k++;
    for(j=1; j<COMPRESSED_VECTOR_SIZE; j++) {
      result_matrix->compressed_values[k] = matrix->compressed_values[k];
      k++;
    }
  }

  return result_matrix;
}


double block_dot_product_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2, int offset_x, int offset_y, int block_pos_1, int block_pos_2) {
  int i, j, k;
  double result;
  double *slope_block_1, *slope_block_2;
  double *block_1, *block_2;

  slope_block_1 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block_2 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  block_1 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  block_2 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  result = 0.0;

  idct(slope_block_1, matrix_1->compressed_values, block_pos_1 * COMPRESSED_VECTOR_SIZE);
  idct(slope_block_2, matrix_2->compressed_values, block_pos_2 * COMPRESSED_VECTOR_SIZE);

  block_1[POS(0, 0, BLOCK_SIZE)] = matrix_1->block_first_elts[block_pos_1];

  for(j=1; j<BLOCK_SIZE; j++) {
    block_1[POS(j, 0, BLOCK_SIZE)] = block_1[POS(j-1, 0, BLOCK_SIZE)] + slope_block_1[POS(j, 0, BLOCK_SIZE)] * matrix_1->block_mean_slope[block_pos_1] ;
  }

  for(i=1; i<=offset_y; i++) {
    block_1[POS(0, i, BLOCK_SIZE)] = block_1[POS(0, i-1, BLOCK_SIZE)] + slope_block_1[POS(0, i, BLOCK_SIZE)] * matrix_1->block_mean_slope[block_pos_1] ;
  }

  for(i=1; i<=offset_y; i++) {
    for(j=1; j<BLOCK_SIZE; j++) {
      block_1[POS(j, i, BLOCK_SIZE)] = (block_1[POS(j-1, i, BLOCK_SIZE)] + block_1[POS(j, i-1, BLOCK_SIZE)]) / 2.0
                                           + slope_block_1[POS(j, i, BLOCK_SIZE)] * matrix_1->block_mean_slope[block_pos_1] ;
    }
  }

  block_2[POS(0, 0, BLOCK_SIZE)] = matrix_2->block_first_elts[block_pos_2];

  for(i=1; i<BLOCK_SIZE; i++) {
    block_2[POS(0, i, BLOCK_SIZE)] = block_2[POS(0, i-1, BLOCK_SIZE)] + slope_block_2[POS(0, i, BLOCK_SIZE)] * matrix_2->block_mean_slope[block_pos_2] ;
  }

  for(j=1; j<=offset_x; j++) {
    block_2[POS(j, 0, BLOCK_SIZE)] = block_2[POS(j-1, 0, BLOCK_SIZE)] + slope_block_2[POS(j, 0, BLOCK_SIZE)] * matrix_2->block_mean_slope[block_pos_2] ;
  }

  for(i=1; i<BLOCK_SIZE; i++) {
    for(j=1; j<=offset_x; j++) {
      block_2[POS(j, i, BLOCK_SIZE)] = (block_2[POS(j-1, i, BLOCK_SIZE)] + block_2[POS(j, i-1, BLOCK_SIZE)]) / 2.0
                                           + slope_block_2[POS(j, i, BLOCK_SIZE)] * matrix_2->block_mean_slope[block_pos_2] ;
    }
  }

  for(k=0; k<BLOCK_SIZE; k++) {
    result += block_1[POS(k, offset_y, BLOCK_SIZE)] * block_2[POS(offset_x, k, BLOCK_SIZE)] ;
  }

  free(slope_block_1);
  free(slope_block_2);
  free(block_1);
  free(block_2);

  return result;
}


double blaz_dot_product_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2, int line, int column) {
  int i, j, blocks_per_line;
  int offset_x, offset_y, block_pos_1, block_pos_2;
  double result;

  i = line / BLOCK_SIZE;
  j = column / BLOCK_SIZE;
  blocks_per_line = matrix_1->width / BLOCK_SIZE;
  block_pos_1 = i * blocks_per_line;
  block_pos_2 = j;

  offset_x = column % BLOCK_SIZE;
  offset_y = line % BLOCK_SIZE;
  result = 0.0;

  for(i=0; i<matrix_1->width / BLOCK_SIZE; i++) {
      result += block_dot_product_compressed(matrix_1, matrix_2, offset_x, offset_y,
                                             block_pos_1 + i, block_pos_2 + i * blocks_per_line);
  }

  return result;
}


Blaz_Compressed_Matrix *blaz_mul_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2) {
  int i, j, k, l, a, b;
  double res;
  double *slope_block;
  Blaz_Matrix *uncompressed_matrix, *uncompressed_matrix_1, *uncompressed_matrix_2;
  Blaz_Compressed_Matrix *compressed_result_matrix;

  uncompressed_matrix = (Blaz_Matrix*)blaz_malloc(sizeof(Blaz_Matrix));
  uncompressed_matrix->matrix = (double*)blaz_malloc(matrix_1->height * matrix_2->width * sizeof(double));
  uncompressed_matrix->width = matrix_2->width;
  uncompressed_matrix->height = matrix_1->height;

  uncompressed_matrix_1 = (Blaz_Matrix*)blaz_malloc(sizeof(Blaz_Matrix));
  uncompressed_matrix_1->matrix = (double*)blaz_malloc(matrix_1->height * matrix_1->width * sizeof(double));
  uncompressed_matrix_1->width = matrix_1->width;
  uncompressed_matrix_1->height = matrix_1->height;

  uncompressed_matrix_2 = (Blaz_Matrix*)blaz_malloc(sizeof(Blaz_Matrix));
  uncompressed_matrix_2->matrix = (double*)blaz_malloc(matrix_2->height * matrix_2->width * sizeof(double));
  uncompressed_matrix_2->width = matrix_2->width;
  uncompressed_matrix_2->height = matrix_2->height;

  slope_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  k = l = 0;

  for(i=0; i<matrix_1->height; i+=BLOCK_SIZE) {
    for(j=0; j<matrix_1->width; j+=BLOCK_SIZE) {
      idct(slope_block, matrix_1->compressed_values, l);

      uncompressed_matrix_1->matrix[POS(j, i, BLOCK_SIZE)] = matrix_1->block_first_elts[k];

      for(a=1; a<BLOCK_SIZE; a++) {
        uncompressed_matrix_1->matrix[POS(j+a, i, matrix_1->width)] = uncompressed_matrix_1->matrix[POS(j+a-1, i, matrix_1->width)]
                                                                    + slope_block[POS(j, 0, BLOCK_SIZE)] * matrix_1->block_mean_slope[k] ;
      }

      for(a=1; a<BLOCK_SIZE; a++) {
        uncompressed_matrix_1->matrix[POS(j, i+a, matrix_1->width)] = uncompressed_matrix_1->matrix[POS(j, i+a-1, matrix_1->width)]
                                                                    + slope_block[POS(j, 0, BLOCK_SIZE)] * matrix_1->block_mean_slope[k] ;
      }

      for(a=1; a<BLOCK_SIZE; a++) {
        for(b=1; b<BLOCK_SIZE; b++) {
          uncompressed_matrix_1->matrix[POS(j+b, i+a, matrix_1->width)] = (uncompressed_matrix_1->matrix[POS(j+b-1, i+a, matrix_1->width)]
                                                                        +  uncompressed_matrix_1->matrix[POS(j+b, i+a-1, matrix_1->width)]) / 2.0
                                                                        + slope_block[POS(j, i, BLOCK_SIZE)] * matrix_1->block_mean_slope[k] ;
        }
      }
      k++;
      l += COMPRESSED_VECTOR_SIZE;
    }
  }

  k = l = 0;

  for(i=0; i<matrix_2->height; i+=BLOCK_SIZE) {
    for(j=0; j<matrix_2->width; j+=BLOCK_SIZE) {
      idct(slope_block, matrix_2->compressed_values, l);

      uncompressed_matrix_2->matrix[POS(j, i, BLOCK_SIZE)] = matrix_2->block_first_elts[k];

      for(a=1; a<BLOCK_SIZE; a++) {
        uncompressed_matrix_2->matrix[POS(j+a, i, matrix_2->width)] = uncompressed_matrix_2->matrix[POS(j+a-1, i, matrix_2->width)]
                                                                    + slope_block[POS(j, 0, BLOCK_SIZE)] * matrix_2->block_mean_slope[k] ;
      }

      for(a=1; a<BLOCK_SIZE; a++) {
        uncompressed_matrix_2->matrix[POS(j, i+a, matrix_1->width)] = uncompressed_matrix_2->matrix[POS(j, i+a-1, matrix_2->width)]
                                                                    + slope_block[POS(j, 0, BLOCK_SIZE)] * matrix_2->block_mean_slope[k] ;
      }

      for(a=1; a<BLOCK_SIZE; a++) {
        for(b=1; b<BLOCK_SIZE; b++) {
          uncompressed_matrix_2->matrix[POS(j+b, i+a, matrix_1->width)] = (uncompressed_matrix_2->matrix[POS(j+b-1, i+a, matrix_2->width)]
                                                                        +  uncompressed_matrix_2->matrix[POS(j+b, i+a-1, matrix_2->width)]) / 2.0
                                                                        + slope_block[POS(j, i, BLOCK_SIZE)] * matrix_2->block_mean_slope[k] ;
        }
      }
      k++;
      l += COMPRESSED_VECTOR_SIZE;
    }
  }

  for(i=0; i<uncompressed_matrix->height; i++) {
    for(j=0; j<uncompressed_matrix->width; j++) {
      res = 0.0;
      for(k=0;k<matrix_1->width;k++) {
          res += uncompressed_matrix_1->matrix[POS(k,i,uncompressed_matrix_1->width)] * uncompressed_matrix_2->matrix[POS(j,k,uncompressed_matrix_2->width)];
      }
      uncompressed_matrix->matrix[POS(j,i,uncompressed_matrix->width)] = res;
    }
  }

  compressed_result_matrix = blaz_compress(uncompressed_matrix);

  free(slope_block);
  free(uncompressed_matrix->matrix);
  free(uncompressed_matrix_1->matrix);
  free(uncompressed_matrix_2->matrix);
  free(uncompressed_matrix);
  free(uncompressed_matrix_1);
  free(uncompressed_matrix_2);

  return compressed_result_matrix;
}
