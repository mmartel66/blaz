
#include <stdlib.h>
#include <math.h>
#include <blaz.h>


Blaz_Matrix *add(Blaz_Matrix *matrix_1, Blaz_Matrix *matrix_2) {
  int i, j;
  Blaz_Matrix *result_matrix;

  result_matrix = (Blaz_Matrix*)malloc(sizeof(Blaz_Matrix));
  result_matrix->width = matrix_1->width;
  result_matrix->height = matrix_1->height;
  result_matrix->matrix = (double*)malloc(result_matrix->width * result_matrix->height * sizeof(double));

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {
      result_matrix->matrix[POS(j,i,result_matrix->width)] = matrix_1->matrix[POS(j,i,result_matrix->width)] + matrix_2->matrix[POS(j,i,result_matrix->width)];
    }
  }
  return result_matrix;
}


Blaz_Matrix *mul_cst(Blaz_Matrix *matrix, double cst) {
  int i, j;
  Blaz_Matrix *result_matrix;

  result_matrix = (Blaz_Matrix*)malloc(sizeof(Blaz_Matrix));
  result_matrix->width = matrix->width;
  result_matrix->height = matrix->height;
  result_matrix->matrix = (double*)malloc(result_matrix->width * result_matrix->height * sizeof(double));

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {
      result_matrix->matrix[POS(j,i,result_matrix->width)] = matrix->matrix[POS(j,i,result_matrix->width)] * cst;
    }
  }
  return result_matrix;
}


Blaz_Compressed_Matrix *add_compressed(Blaz_Compressed_Matrix *matrix_1, Blaz_Compressed_Matrix *matrix_2) {
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
    (result_matrix->block_first_elts)[i] = (matrix_1->block_first_elts)[i] + (matrix_2->block_first_elts)[i];
    (result_matrix->block_mean_slope)[i] = (matrix_1->block_mean_slope)[i] + (matrix_2->block_mean_slope)[i];
  }

  k = 0;

  for(i=0; i < result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    coef_1 = ((matrix_1->block_mean_slope)[i]) / ((matrix_1->block_mean_slope)[i] + (matrix_2->block_mean_slope)[i]);
    coef_2 = ((matrix_2->block_mean_slope)[i]) / ((matrix_1->block_mean_slope)[i] + (matrix_2->block_mean_slope)[i]);
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

/*
  for(i=0; i <result_matrix->width * result_matrix->height / (BLOCK_SIZE * BLOCK_SIZE); i++) {
    if (matrix_1->block_mean_slope[i] > matrix_2->block_mean_slope[i]) {
      (result_matrix->block_mean_slope)[i] = (matrix_1->block_mean_slope)[i];
      coef_1 = (matrix_2->block_mean_slope[i]) / (matrix_1->block_mean_slope[i]);
      for(j=0; j<COMPRESSED_VECTOR_SIZE; j++) {
        (result_matrix->compressed_values)[k] = (s_8)((double)(matrix_1->compressed_values)[k] + coef_1 * (double)(matrix_2->compressed_values)[k]);
printf(">>>%d + %f * %d = %d \n",matrix_1->compressed_values[k],coef_1,matrix_2->compressed_values[k],result_matrix->compressed_values[k]);
        k++;
      }
    } else {
      (result_matrix->block_mean_slope)[i] = (matrix_2->block_mean_slope)[i];
      coef_1 = (matrix_1->block_mean_slope[i]) / (matrix_2->block_mean_slope[i]);
      for(j=0; j<COMPRESSED_VECTOR_SIZE; j++) {
        (result_matrix->compressed_values)[k] = (s_8)((double)(matrix_2->compressed_values)[k] + coef_1 * (double)(matrix_1->compressed_values)[k]);
        printf(">>>%d + %f * %d = %d \n",matrix_2->compressed_values[k],coef_1,matrix_1->compressed_values[k],result_matrix->compressed_values[k]);

        k++;
      }
    }
  }*/

  return result_matrix;
}
