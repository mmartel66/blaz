/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : operations.c
 * @Role ........ : operations on matrices
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <math.h>
#include <blaz.h>


Blaz_Matrix *blaz_add(Blaz_Matrix *matrix_1, Blaz_Matrix *matrix_2) {
  int i, j;
  Blaz_Matrix *result_matrix;

  result_matrix = (Blaz_Matrix*)malloc(sizeof(Blaz_Matrix));
  result_matrix->width = matrix_1->width;
  result_matrix->height = matrix_1->height;
  result_matrix->matrix = (double*)malloc(result_matrix->width * result_matrix->height * sizeof(double));

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {
      result_matrix->matrix[POS(j, i, result_matrix->width)] = matrix_1->matrix[POS(j, i, result_matrix->width)] + matrix_2->matrix[POS(j, i, result_matrix->width)];
    }
  }
  return result_matrix;
}


Blaz_Matrix *blaz_mul_cst(Blaz_Matrix *matrix, double cst) {
  int i, j;
  Blaz_Matrix *result_matrix;

  result_matrix = (Blaz_Matrix*)malloc(sizeof(Blaz_Matrix));
  result_matrix->width = matrix->width;
  result_matrix->height = matrix->height;
  result_matrix->matrix = (double*)malloc(result_matrix->width * result_matrix->height * sizeof(double));

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {
      result_matrix->matrix[POS(j, i, result_matrix->width)] = matrix->matrix[POS(j, i, result_matrix->width)] * cst;
    }
  }
  return result_matrix;
}


double blaz_dot_product(Blaz_Matrix *matrix_1, Blaz_Matrix *matrix_2, int line, int column) {
  int i;
  double result;

  result = 0.0;

  for(i=0; i<matrix_1->width; i++) {
      result += matrix_1->matrix[POS(i, line, matrix_1->width)] * matrix_2->matrix[POS(column, i, matrix_2->width)];
  }
  
  return result;
}
