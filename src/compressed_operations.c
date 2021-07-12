/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : compressed_operations.c
 * @Role ........ : operations on compressed matrices
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/07/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <math.h>
#include <blaz.h>


double blaz_get_compressed_matrix_elt(Blaz_Compressed_Matrix *compressed_matrix, int column, int line) {
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

  return uncompressed_block[POS(offset_x, offset_y, BLOCK_SIZE)];
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
  int i, j;
  double result, delta_1, delta_2;
  double *slope_block_1, *slope_block_2;

  slope_block_1 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block_2 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  delta_1 = delta_2 = 0.0;
  result = matrix_1->block_first_elts[block_pos_1] * matrix_2->block_first_elts[block_pos_2] * BLOCK_SIZE;

  idct(slope_block_1, matrix_1->compressed_values, block_pos_1 * COMPRESSED_VECTOR_SIZE);
  idct(slope_block_2, matrix_2->compressed_values, block_pos_2 * COMPRESSED_VECTOR_SIZE);

  for(i=1; i<BLOCK_SIZE; i++) {
    delta_1 += slope_block_1[POS(i, offset_y, BLOCK_SIZE)] * matrix_1->block_mean_slope[block_pos_1];
    delta_2 += slope_block_2[POS(offset_x, i, BLOCK_SIZE)] * matrix_2->block_mean_slope[block_pos_2];
    result += matrix_2->block_first_elts[block_pos_2] * delta_1;
    result += matrix_1->block_first_elts[block_pos_1] * delta_2;
    result += delta_1 * delta_2;
  }

  return result;
}


double blaz_dot_product_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2, int line, int column) {
  int i, j;
  int offset_x, offset_y, block_pos_1, block_pos_2, compressed_values_pos_1, compressed_values_pos_2;
  double result;

  block_pos_1 = line / BLOCK_SIZE;
  block_pos_2 = column / BLOCK_SIZE;
  offset_x = column % BLOCK_SIZE;
  offset_y = line % BLOCK_SIZE;
  result = 0.0;

  for(i=1; i<matrix_1->width / BLOCK_SIZE; i++) {
    for(j=1; j<matrix_2->height / BLOCK_SIZE; j++) {
      result += block_dot_product_compressed(matrix_1, matrix_2, offset_x, offset_y,
                                             block_pos_1 + j, block_pos_2 + i * matrix_2->width / BLOCK_SIZE);
    }
  }

  return result;
}
