/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : dot_product.c
 * @Role ........ : Dot product of a line and column of
                    two matrices given in files.
                    File names given as arguments in the
                    command line (argv[1] and argv[2]).
                    Comparison between the dot product of
                    uncompressed matrices and the dot
                    product of the corresponding compressed
                    matrices.
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/03/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <float.h>
#include <blaz.h>


double fabs(double x) {
  double y;
  if (x<0) y = -x; else y =x;
  return y;
}


int main(int argc, char **argv) {
  int i, j;
  Blaz_Matrix *matrix_1, *matrix_2, *uncompressed_matrix_1, *uncompressed_matrix_2;
  Blaz_Compressed_Matrix *compressed_matrix_1, *compressed_matrix_2;
  double result, compressed_result;

  double abs_err, rel_err, e, mean_rel_err;
  double abs_err1, rel_err1, e1, mean_rel_err1;
  double abs_err2, rel_err2, e2, mean_rel_err2;
  int ia, ja, ir, jr;

  matrix_1 = blaz_read_matrix(argv[1]);
  matrix_2 = blaz_read_matrix(argv[2]);

  compressed_matrix_1 = blaz_compress(matrix_1);
  compressed_matrix_2 = blaz_compress(matrix_2);

  result = blaz_dot_product(matrix_1, matrix_2, 0, 0);
  compressed_result = blaz_dot_product_compressed(compressed_matrix_1, compressed_matrix_2, 0, 0);

  uncompressed_matrix_1 = blaz_uncompress(compressed_matrix_1);
  uncompressed_matrix_2 = blaz_uncompress(compressed_matrix_2);

  abs_err = rel_err = DBL_MIN;
  abs_err1 = rel_err1 = DBL_MIN;
  abs_err2 = rel_err2 = DBL_MIN;
  mean_rel_err = mean_rel_err1 = mean_rel_err2 = 0.0;

  for(i=0; i<matrix_1->height; i++) {
    for(j=0; j<matrix_1->width; j++) {
      e1 = fabs(matrix_1->matrix[POS(j,i,matrix_1->width)] - uncompressed_matrix_1->matrix[POS(j,i,matrix_1->width)]);
      if (e1 > abs_err1) { abs_err1 = e1; };
      e1 /= fabs(matrix_1->matrix[POS(j,i,matrix_1->width)]);
      if ((matrix_1->matrix[POS(j,i,matrix_1->width)] != 0)) { mean_rel_err1 += e1; };
      if ((e1 > rel_err1) & (matrix_1->matrix[POS(j,i,matrix_1->width)] != 0)) { rel_err1 = e1; };

      e2 = fabs(matrix_2->matrix[POS(j,i,matrix_1->width)] - uncompressed_matrix_2->matrix[POS(j,i,matrix_1->width)]);
      if (e2 > abs_err2) { abs_err2 = e2; };
      e2 /= fabs(matrix_2->matrix[POS(j,i,matrix_1->width)]);
      if ((matrix_2->matrix[POS(j,i,matrix_1->width)] != 0)) { mean_rel_err2 += e2; };
      if ((e2 > rel_err2) & (matrix_2->matrix[POS(j,i,matrix_1->width)] != 0)) { rel_err2 = e2; };
    }
  }

  mean_rel_err1 /= (matrix_1->width * matrix_1->height);
  mean_rel_err2 /= (matrix_1->width * matrix_1->height);

  printf("Matrix 1:\n");
  printf("Worst absolute error: %f\n", abs_err1);
  printf("Worst relative error: %f\n", rel_err1);
  printf("Mean relative error: %f\n", mean_rel_err1);

  printf("Matrix 2:\n");
  printf("Worst absolute error: %f\n", abs_err2);
  printf("Worst relative error: %f\n", rel_err2);
  printf("Mean relative error: %f\n", mean_rel_err2);

  printf("\nDot product: %f, compressed dot product: %f\n", result, compressed_result);
  printf("Absolute error: %f\n", fabs(result - compressed_result));
  printf("Relative error: %f\n", fabs((result - compressed_result) / result));
}

/*

set grid
set view 60,320
set xtics font ",11"
set ytics font ",11"
set ztics font ",11"
set ytics offset 4
set xtics offset -4
splot 'ddd' with points pointtype 7 lc rgb "#ff7c00" title "Original", 'ccc' with points pointtype 7 lc rgb "#53ae32" title "Compressed/Uncompressed"

*/
