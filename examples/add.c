/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : add.c
 * @Role ........ : Addition of two matrices given in files.
                    File names given as arguments in the
                    command line (argv[1] and argv[2]).
                    Comparison between the addition of
                    uncompressed matrices and the addition of
                    corresponding compressed matrices.
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 22/11/2021
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
  Blaz_Matrix *result_matrix, *matrix_1, *matrix_2, *uncompressed_result_matrix, *uncompressed_matrix_1, *uncompressed_matrix_2;
  Blaz_Compressed_Matrix *compressed_result_matrix, *compressed_matrix_1, *compressed_matrix_2;

  double abs_err, rel_err, e, mean_rel_err;
  double abs_err1, rel_err1, e1, mean_rel_err1;
  double abs_err2, rel_err2, e2, mean_rel_err2;
  int ia, ja, ir, jr;

  matrix_1 = blaz_read_matrix(argv[1]);
  matrix_2 = blaz_read_matrix(argv[2]);

  compressed_matrix_1 = blaz_compress(matrix_1);
  compressed_matrix_2 = blaz_compress(matrix_2);

  result_matrix = blaz_add(matrix_1, matrix_2);
  compressed_result_matrix = blaz_add_compressed(compressed_matrix_1, compressed_matrix_2);

  uncompressed_matrix_1 = blaz_uncompress(compressed_matrix_1);
  uncompressed_matrix_2 = blaz_uncompress(compressed_matrix_2);
  uncompressed_result_matrix = blaz_uncompress(compressed_result_matrix);

  abs_err = rel_err = DBL_MIN;
  abs_err1 = rel_err1 = DBL_MIN;
  abs_err2 = rel_err2 = DBL_MIN;
  mean_rel_err = mean_rel_err1 = mean_rel_err2 = 0.0;

  for(i=0; i<result_matrix->height; i++) {
    for(j=0; j<result_matrix->width; j++) {

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

      e2 = fabs(matrix_2->matrix[POS(j,i,result_matrix->width)] - uncompressed_matrix_2->matrix[POS(j,i,result_matrix->width)]);
      if (e2 > abs_err2) { abs_err2 = e2; };
      e2 /= fabs(matrix_2->matrix[POS(j,i,result_matrix->width)]);
      if ((matrix_2->matrix[POS(j,i,result_matrix->width)] != 0)) { mean_rel_err2 += e2; };
      if ((e2 > rel_err2) & (matrix_2->matrix[POS(j,i,result_matrix->width)] != 0)) { rel_err2 = e2; };
    }
  }

  mean_rel_err /= (result_matrix->width * result_matrix->height);
  mean_rel_err1 /= (result_matrix->width * result_matrix->height);
  mean_rel_err2 /= (result_matrix->width * result_matrix->height);

  printf("Matrix 1:\n");
  printf("Worst absolute error: %f\n", abs_err1);
  printf("Worst relative error: %f\n", rel_err1);
  printf("Mean relative error: %f\n", mean_rel_err1);

  printf("Matrix 2:\n");
  printf("Worst absolute error: %f\n", abs_err2);
  printf("Worst relative error: %f\n", rel_err2);
  printf("Mean relative error: %f\n", mean_rel_err2);

  printf("Matrix 1 + Matrix 2:\n");
  printf("Worst absolute error: %f (%d,%d) original=%f compressed=%f\n", abs_err, ja, ia, result_matrix->matrix[POS(ja,ia,result_matrix->width)], uncompressed_result_matrix->matrix[POS(ja,ia,result_matrix->width)]);
  printf("Worst relative error: %f (%d,%d) original=%f compressed=%f\n", rel_err, jr ,ir, result_matrix->matrix[POS(jr,ir,result_matrix->width)], uncompressed_result_matrix->matrix[POS(jr,ir,result_matrix->width)]);
  printf("Mean relative error: %f\n", mean_rel_err);

  for(i=0; i<uncompressed_result_matrix->height; i++) {
    for(j=0; j<uncompressed_result_matrix->width; j++) {
      printf("%f %f %f\n", 0.1*j, 0.1*i, uncompressed_result_matrix->matrix[POS(j, i, uncompressed_result_matrix->width)]);
    }
  }
}
