/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : dct.c
 * @Role ........ : discrete cosine transform and
                    quantization
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/07/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <blaz.h>

double dct_coef[64] = {
                        0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534, 0.3535534,
                        0.490393, 0.415735, 0.277785, 0.0975452, -0.0975452, -0.277785, -0.415735, -0.490393,
                        0.46194, 0.191342, -0.191342, -0.46194, -0.46194, -0.191342, 0.191342, 0.46194,
                        0.415735, -0.0975452, -0.490393, -0.277785, 0.277785, 0.490393, 0.0975452, -0.415735,
                        0.353553, -0.353553, -0.353553, 0.353553, 0.353553, -0.353553, -0.353553, 0.353553,
                        0.277785, -0.490393, 0.0975452, 0.415735, -0.415735, -0.0975452, 0.490393, -0.277785,
                        0.191342, -0.46194, 0.46194, -0.191342, -0.191342, 0.46194, -0.46194, 0.191342,
                        0.0975452, -0.277785, 0.415735, -0.490393, 0.490393, -0.415735, 0.277785, -0.0975452
                      };

double dct_coef_transpose[64] = {
                        0.353553, 0.490393, 0.461940, 0.415735, 0.353553, 0.277785, 0.191342, 0.097545,
                        0.353553, 0.415735, 0.191342, -0.097545, -0.353553, -0.490393, -0.461940, -0.277785,
                        0.353553, 0.277785, -0.191342, -0.490393, -0.353553, 0.097545, 0.461940, 0.415735,
                        0.353553, 0.097545, -0.461940, -0.277785, 0.353553, 0.415735, -0.191342, -0.490393,
                        0.353553, -0.097545, -0.461940, 0.277785, 0.353553, -0.415735, -0.191342, 0.490393,
                        0.353553, -0.277785, -0.191342, 0.490393, -0.353553, -0.097545, 0.461940, -0.415735,
                        0.353553, -0.415735, 0.191342, 0.097545, -0.353553, 0.490393, -0.461940, 0.277785,
                        0.353553, -0.490393, 0.461940, -0.415735, 0.353553, -0.277785, 0.191342, -0.097545
                      };


double *dct_block_matrix_multiply(double *block, double *matrix) {
  double *result_block;
  double res;
  int i,j,k;

  result_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      res = 0.0;
      for(k=0; k<BLOCK_SIZE; k++) {
        res += block[POS(k, i, BLOCK_SIZE)] * matrix[POS(j, k, BLOCK_SIZE)];
      }
      result_block[POS(j, i, BLOCK_SIZE)] = res;
    }
  }
  return result_block;
}


void dct_matrix_block_multiply(double *block, double *matrix, s_8 *quantize_vector, int offset) {
  s_8 *result_block;
  s_8 quantize_coef;
  double *tmp_block;
  double max_elt, res;
  int i, j, k;

  tmp_block = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  result_block = (s_8*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(s_8));
  max_elt = DBL_MIN;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      res = 0.0;
      for(k=0; k<BLOCK_SIZE; k++) {
        res += matrix[POS(k, i, BLOCK_SIZE)] * block[POS(j, k, BLOCK_SIZE)];
      }
      tmp_block[POS(j, i, BLOCK_SIZE)] = res;
      if (fabs(res)>max_elt) max_elt = fabs(res);
    }
  }

  quantize_coef = (s_8)(127.0 / max_elt);

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      result_block[POS(j, i, BLOCK_SIZE)] = (s_8)(tmp_block[POS(j, i, BLOCK_SIZE)] * (double)quantize_coef);
    }
  }

  quantize_vector[offset] = quantize_coef;
  for(i=0; i<BLOCK_SIZE; i++) {
    quantize_vector[offset+i+1] = result_block[POS(i, 0, BLOCK_SIZE)];
    quantize_vector[offset+i+9] = result_block[POS(i, 1, BLOCK_SIZE)];
  }
  for(i=0; i<BLOCK_SIZE - 2; i++) {
    quantize_vector[offset+i+17] = result_block[POS(0, i+2, BLOCK_SIZE)];
    quantize_vector[offset+i+23] = result_block[POS(1, i+2, BLOCK_SIZE)];
  }

  free(tmp_block);
  free(result_block);
}


void idct_block_matrix_multiply(double *block, double *matrix, double *result_block, double quantize_coef) {
  double res;
  int i, j, k;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      res = 0.0;
      for(k=0; k<BLOCK_SIZE; k++) {
        res += block[POS(k, i, BLOCK_SIZE)] * matrix[POS(j, k, BLOCK_SIZE)];
      }
      result_block[POS(j, i, BLOCK_SIZE)] = res / quantize_coef;
    }
  }
}


double *idct_matrix_block_multiply(double *block, double *matrix, double *result_matrix) {
  int i, j, k;
  double res;

  for(i=0; i<BLOCK_SIZE; i++) {
    for(j=0; j<BLOCK_SIZE; j++) {
      res = 0.0;
      for(k=0; k<BLOCK_SIZE; k++) {
        res += matrix[POS(k, i, BLOCK_SIZE)] * block[POS(j, k, BLOCK_SIZE)];
      }
      result_matrix[POS(j, i, BLOCK_SIZE)] = res;
    }
  }
}


void dct(double *matrix, s_8 *quantized_vector, int offset) {
  double *block_0;

  block_0 = dct_block_matrix_multiply(dct_coef, matrix);

  dct_matrix_block_multiply(dct_coef_transpose, block_0, quantized_vector, offset);

  free(block_0);
}


void idct(double *matrix, s_8 *quantized_vector, int offset) {
  double * block_0, *block_1;
  double quantize_coef;
  int i, j, k;

  block_0 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));
  block_1 = (double*)blaz_malloc(BLOCK_SIZE * BLOCK_SIZE * sizeof(double));

  quantize_coef = (double)quantized_vector[offset];

  k = 1;
  for(i=0; i<BLOCK_SIZE; i++) {
      block_0[POS(i, 0, BLOCK_SIZE)] = (double)quantized_vector[offset + k];
      block_0[POS(i, 1, BLOCK_SIZE)] = (double)quantized_vector[offset + k + BLOCK_SIZE];
      k++;
  }

  k = 2 * BLOCK_SIZE + 1;
  for(i=2; i<BLOCK_SIZE; i++) {
    block_0[POS(0, i, BLOCK_SIZE)] = (double)quantized_vector[offset + k];
    block_0[POS(1, i, BLOCK_SIZE)] = (double)quantized_vector[offset + k + BLOCK_SIZE - 2];
    k++;
  }

  for(i=2; i<BLOCK_SIZE; i++) {
    for(j=2; j<BLOCK_SIZE; j++) {
      block_0[POS(j, i, BLOCK_SIZE)] = 0.0;
    }
  }

  idct_block_matrix_multiply(dct_coef_transpose, block_0, block_1, quantize_coef);
  idct_matrix_block_multiply(dct_coef, block_1, matrix);

  free(block_0);
  free(block_1);
}
