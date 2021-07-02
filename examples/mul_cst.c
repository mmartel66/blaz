/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : mul_cst.c
 * @Role ........ : Multiplication of a matrix given in file.
                    File name given as argument in the
                    command line (argv[1]).
                    Comparison between the multiplication of
                    uncompressed matrix and the multiplication
                    of the corresponding compressed matrix.
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
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
  Blaz_Matrix *result_matrix, *matrix_1, *uncompressed_result_matrix, *uncompressed_matrix_1;
  Blaz_Compressed_Matrix *compressed_result_matrix, *compressed_matrix_1;
  double abs_err, rel_err, e, mean_rel_err;
  double abs_err1, rel_err1, e1, mean_rel_err1;
  int ia, ja, ir, jr;
  double cst;

  cst = 1.9;

  matrix_1 = read_blaz_matrix(argv[1]);

  compressed_matrix_1 = blaz_compress(matrix_1);

  result_matrix = blaz_mul_cst(matrix_1, cst);
  compressed_result_matrix = blaz_mul_cst_compressed(compressed_matrix_1, cst);

  uncompressed_matrix_1 = blaz_uncompress(compressed_matrix_1);
  uncompressed_result_matrix = blaz_uncompress(compressed_result_matrix);

  abs_err = rel_err = DBL_MIN;
  abs_err1 = rel_err1 = DBL_MIN;
  mean_rel_err = mean_rel_err1 = 0.0;

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {
      printf("%f %f %f\n",0.1 * (double)j, 0.1 * (double)i, result_matrix->matrix[POS(j,i,result_matrix->width)]);

      e = fabs(result_matrix->matrix[POS(j,i,result_matrix->width)] - uncompressed_result_matrix->matrix[POS(j,i,result_matrix->width)]);
      if (e > abs_err) { abs_err = e; ia = i; ja = j; };

      e /= fabs(result_matrix->matrix[POS(j,i,result_matrix->width)]);
      if ((result_matrix->matrix[POS(j,i,result_matrix->width)] != 0)) { mean_rel_err += e; };
      if ((e > rel_err) & (result_matrix->matrix[POS(j,i,result_matrix->width)] != 0)) { rel_err = e; ir = i; jr = j; };

      e1 = fabs(matrix_1->matrix[POS(j,i,result_matrix->width)] - uncompressed_matrix_1->matrix[POS(j,i,result_matrix->width)]);
      if (e1 > abs_err1) { abs_err1 = e1; };
      e1 /= fabs(matrix_1->matrix[POS(j,i,result_matrix->width)]);
      if ((matrix_1->matrix[POS(j,i,result_matrix->width)] != 0)) { mean_rel_err1 += e1; };
      if ((e1 > rel_err1) & (matrix_1->matrix[POS(j,i,result_matrix->width)] != 0)) { rel_err1 = e1; };
    }
  }

  mean_rel_err /= (result_matrix->width * result_matrix->height);
  mean_rel_err1 /= (result_matrix->width * result_matrix->height);

  printf("Matrix:\n");
  printf("Worst absolute error: %f\n", abs_err1);
  printf("Worst relative error: %f\n", rel_err1);
  printf("Mean relative error: %f\n", mean_rel_err1);

  printf("Matrix * %f:\n",cst);
  printf("Worst absolute error: %f (%d,%d) original=%f compressed=%f\n", abs_err, ja, ia, result_matrix->matrix[POS(ja,ia,result_matrix->width)], uncompressed_result_matrix->matrix[POS(ja,ia,result_matrix->width)]);
  printf("Worst relative error: %f (%d,%d) original=%f compressed=%f\n", rel_err, jr ,ir, result_matrix->matrix[POS(jr,ir,result_matrix->width)], uncompressed_result_matrix->matrix[POS(jr,ir,result_matrix->width)]);
  printf("Mean relative error: %f\n", mean_rel_err);
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
