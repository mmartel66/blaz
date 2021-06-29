
#include <math.h>
#include <stdlib.h>
#include <compress.h>


int get_exponent(double x) {
  int exp;

  frexpf(x, &exp);

  return exp;
}


int block_max_exponent(double *matrix, int block_x, int block_y, int block_size, int matrix_width) {
  int i, j, e, max_exp;

  max_exp = -1023;

  for(i=0; i<block_size; i++) {
    for(j=0; j<block_size; j++) {
      if (matrix[POS(block_x + j, block_y + i, matrix_width)] != 0.0) {
        e = get_exponent(POS(block_x + j, block_y + i, matrix_width));
        if (e > max_exp) max_exp = e;
      }
    }
  }

  if (max_exp == -1023) max_exp = 0;

  return max_exp;
}


u_64 get_mantissa_with_hidden_bit(double x) {
  u_64 m;

  m = *((u_64 *)&x);
  m = (m & 0xFFFFFFFFFFFFF);
  if (x != 0.0) m = (m | 0x10000000000000); // add hidden bit

  return m;
}


int block_align_mantissa(double *matrix, u_64 *result_matrix, int block_x, int block_y, int block_size, int matrix_width) {
  int i, j, max_exp, e, delta_e;
  u_64 mantissa;
  char *mantissaString;

  max_exp = block_max_exponent(matrix, block_x, block_y, block_size, matrix_width);

  for(i=0; i<block_size; i++) {
    for(j=0; j<block_size; j++) {
      e = get_exponent(matrix[POS(block_x + j, block_y + i, matrix_width)]);
      delta_e = abs(e - max_exp);
      mantissa = get_mantissa_with_hidden_bit(matrix[POS(block_x + j, block_y + i, matrix_width)]);
      mantissa = mantissa >> delta_e;
      result_matrix[POS(block_x + j, block_y + i, matrix_width)] = mantissa;
    }
  }

  return max_exp;
}

// block_unalign_mantissa(aligned_matrix, block_exponents_vector[m], delta_matrix, j, i, 8, matrix_width);

double unalign_mantissa(u_64 x, u_16 exponent) {
  int count;
  u_64 new_exponent, r;
  double result;

  count = 0;

  while((x & 0x10000000000000) == 0) {
    x = (x << 1);
    count++;
  }

  x = (x & 0xFFFFFFFFFFFFF);
  new_exponent =  count - exponent + 1023;
  new_exponent = (new_exponent << 52);
  r = (new_exponent | x);
  result = *((double *)&r);

  return result;
}


void block_unalign_mantissa(u_64 *matrix, u_16 exponent, double *result_matrix, int block_x, int block_y, int block_size, int matrix_width) {
  int i, j;

  for(i=0; i<block_size; i++) {
    for(j=0; j<block_size; j++) {
      result_matrix[POS(block_x+j,block_y+i,matrix_width)] = unalign_mantissa(matrix[POS(block_x+j,block_y+i,matrix_width)],exponent);
    }
  }
}
