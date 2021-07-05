/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : compressed_operations.c
 * @Role ........ : operations on compressed matrices
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/02/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <math.h>
#include <blaz.h>


Blaz_Compressed_Matrix *blaz_add_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2) {
  int i, j, k;
  double coef_1, coef_2;
  Blaz_Compressed_Matrix *result_matrix;

  result_matrix = (Blaz_Compressed_Matrix*)malloc(sizeof(Blaz_Compressed_Matrix));

  result_matrix->width = matrix_1->width;
  result_matrix->height = matrix_1->height;

  result_matrix->block_first_elts = (double*)malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->block_mean_slope = (double*)malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->compressed_values = (s_8*)malloc(result_matrix->width * result_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

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


Blaz_Compressed_Matrix *blaz_mul_cst_compressed(Blaz_Compressed_Matrix *matrix, double cst) {
  int i, j, k;
  Blaz_Compressed_Matrix *result_matrix;

  result_matrix = (Blaz_Compressed_Matrix*)malloc(sizeof(Blaz_Compressed_Matrix));

  result_matrix->width = matrix->width;
  result_matrix->height = matrix->height;

  result_matrix->block_first_elts = (double*)malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->block_mean_slope = (double*)malloc(result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  result_matrix->compressed_values = (s_8*)malloc(result_matrix->width * result_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

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


s_8 block_elt(s_8 *compressed_values, int block_x, int block_y, int offset_x, int offset_y, int matrix_width) {

  if (offset_y < 2) {
    printf(">>> %d %d\n", (block_y * matrix_width + block_x) / BLOCK_SIZE * COMPRESSED_VECTOR_SIZE + offset_y * BLOCK_SIZE + offset_x + 1,compressed_values[(block_y * matrix_width + block_x) / BLOCK_SIZE * COMPRESSED_VECTOR_SIZE + offset_y * BLOCK_SIZE + offset_x + 1]);
    return compressed_values[(block_y * matrix_width + block_x) / BLOCK_SIZE * COMPRESSED_VECTOR_SIZE + offset_y * BLOCK_SIZE + offset_x + 1];
  } else {
    if (offset_x < 2) {
      return compressed_values[2 * (BLOCK_SIZE + (offset_y - 2)) + offset_x + 1];
    } else {
        return 0;
    }
  }
}

// AA00A6X261

double block_dot_product_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2, int offset_x, int offset_y, int block_pos_1, int block_pos_2, int compressed_values_pos_1, int compressed_values_pos_2) {
  int i, j;
  double result, base, base_1, base_2, delta_1, delta_2;
  double *slope_block_1, *slope_block_2;

  slope_block_1 = (double*)malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  slope_block_2 = (double*)malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  delta_1 =delta_2 = 0.0;
  base = matrix_1->block_mean_slope[block_pos_1] * matrix_2->block_mean_slope[block_pos_2];
  base_1 = matrix_1->block_first_elts[block_pos_1] * matrix_2->block_mean_slope[block_pos_2];
  base_2 = matrix_2->block_first_elts[block_pos_2] * matrix_1->block_mean_slope[block_pos_1];
  result = base_1 * base_2 * BLOCK_SIZE;

  printf("offx=%d offy=%d bpx=%d bpy=%d cv1=%d cv2=%d\n",offset_x,offset_y,block_pos_1,block_pos_2,compressed_values_pos_1,compressed_values_pos_2);

  idct(slope_block_1, matrix_1->compressed_values, compressed_values_pos_1);
  idct(slope_block_2, matrix_2->compressed_values, compressed_values_pos_2);

  for(i=1; i<BLOCK_SIZE; i++) {
    delta_1 += slope_block_1[POS(i, offset_y, BLOCK_SIZE)];
    delta_2 += slope_block_2[POS(offset_x, i, BLOCK_SIZE)];
    result += base_1 * delta_1;
    result += base_2 * delta_2;
    result += base * delta_1 * delta_2;
//printf("result += %f * %f = %f\n",slope_block_2[POS(offset_x, i, BLOCK_SIZE)]*matrix_2->block_mean_slope[block_pos_2],matrix_1->block_mean_slope[block_pos_1]*slope_block_1[POS(offset_x, i, BLOCK_SIZE)],result);
  }
//printf("RRRRRESSSSULT=%f\n",result);
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

  for(i=0; i<matrix_1->width / BLOCK_SIZE; i++) {
    for(j=0; j<matrix_2->height / BLOCK_SIZE; j++) {
      result += block_dot_product_compressed(matrix_1, matrix_2,
                                             offset_x, offset_y,
                                             block_pos_1 + i * matrix_1->width / BLOCK_SIZE,
                                             block_pos_2 + j * matrix_2->width / BLOCK_SIZE,
                                             block_pos_1 + i * matrix_1->width / BLOCK_SIZE * COMPRESSED_VECTOR_SIZE,
                                             block_pos_2 + j * matrix_2->width / BLOCK_SIZE * COMPRESSED_VECTOR_SIZE);
    }
  }

  return result;
}
