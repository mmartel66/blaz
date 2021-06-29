
#include <stdio.h>
#include <float.h>
#include <compress.h>







double fabs(double x) {
  double y;
  if (x<0) y = -x; else y =x;
  return y;
}


int main(int argc, char **argv) {
  int width, height, width1, height1, width2, height2;
  int i, j, mcpt;
  double *matrix, *matrix1, *matrix2, *matrix3, *um1, *um2;
  double abs_err, rel_err, e, mean_rel_err;
  double abs_err1, rel_err1, e1, mean_rel_err1;
  double abs_err2, rel_err2, e2, mean_rel_err2;

  int ia, ja, ir, jr;
  struct Compressed_Matrix *cm, *cm1, *cm2;

  read_matrix(argv[1], &matrix1, &width1, &height1);
  read_matrix(argv[2], &matrix2, &width2, &height2);

  cm1 = compress(matrix1, width1, height1);
  cm2 = compress(matrix2, width2, height2);

  write_compressed_matrix(argv[1], cm1);
  write_compressed_matrix(argv[2], cm2);

  read_compressed_matrix(argv[1], cm1);
  read_compressed_matrix(argv[2], cm2);

  cm = add_compressed(cm1, cm2);

  matrix = add(matrix1, matrix2, width1, height1);

  uncompress(&um1, &width, &height, cm1);
  uncompress(&um2, &width, &height, cm2);
  uncompress(&matrix3, &width, &height, cm);

  abs_err = rel_err = DBL_MIN;
  abs_err1 = rel_err1 = DBL_MIN;
  abs_err2 = rel_err2 = DBL_MIN;
  mean_rel_err = 0.0;
  mean_rel_err1 = 0.0;
  mean_rel_err2 = 0.0;

  for(i=0; i<height; i++) {
    for(j=0; j<width; j++) {
      printf("%f %f %f\n",0.1 * (double)j, 0.1 * (double)i, matrix3[POS(j,i,width)]);

      e = fabs(matrix[POS(j,i,width)] - matrix3[POS(j,i,width)]);
      if (e>abs_err) { abs_err = e; ia =i; ja = j; };
      e /= fabs(matrix[POS(j,i,width)]);
      if ((matrix[POS(j,i,width)]!=0)) { mean_rel_err += e; mcpt++; };
      if ((e>rel_err)&(matrix[POS(j,i,width)]!=0)) { rel_err = e; ir = i; jr = j; };

      e1 = fabs(matrix1[POS(j,i,width)] - um1[POS(j,i,width)]);
      if (e1>abs_err1) { abs_err1 = e1; };
      e1 /= fabs(matrix1[POS(j,i,width)]);
      if ((matrix1[POS(j,i,width)]!=0)) { mean_rel_err1 += e1; };
      if ((e1>rel_err1)&(matrix1[POS(j,i,width)]!=0)) { rel_err1 = e1; };

      e2 = fabs(matrix2[POS(j,i,width)] - um2[POS(j,i,width)]);
      if (e2>abs_err2) { abs_err2 = e2; };
      e2 /= fabs(matrix2[POS(j,i,width)]);
      if ((matrix2[POS(j,i,width)]!=0)) { mean_rel_err2 += e2; };
      if ((e2>rel_err2)&(matrix2[POS(j,i,width)]!=0)) { rel_err2 = e2; };

    }
  }
  mean_rel_err /= (width * height);
  mean_rel_err1 /= (width * height);
  mean_rel_err2 /= (width * height);

  printf("Matrix 1:\n");
  printf("Worst absolute error: %f\n",abs_err1);
  printf("Worst relative error: %f\n",rel_err1);
  printf("Mean relative error: %f\n",mean_rel_err1);

  printf("Matrix 2:\n");
  printf("Worst absolute error: %f\n",abs_err2);
  printf("Worst relative error: %f\n",rel_err2);
  printf("Mean relative error: %f\n",mean_rel_err2);

  printf("Matrix 1 + Matrix 2:\n");
  printf("Worst absolute error: %f (%d,%d)  orig=%f comp=%f\n",abs_err,ja,ia,matrix[POS(ja,ia,width)],matrix3[POS(ja,ia,width)]);
  printf("Worst relative error: %f (%d,%d)  orig=%f comp=%f\n",rel_err,jr,ir,matrix[POS(jr,ir,width)],matrix3[POS(jr,ir,width)]);
  printf("Mean relative error: %f\n",mean_rel_err);

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
