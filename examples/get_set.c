/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : get_set.c
 * @Role ........ : Get and set an element in a matrix
                    given in file. File name given as
                    argument in the command line (argv[1]).
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/09/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <blaz.h>


int main(int argc, char **argv) {
  int i, j;
  Blaz_Matrix *matrix, *uncompressed_matrix;
  Blaz_Compressed_Matrix *compressed_matrix;

  matrix = blaz_read_matrix(argv[1]);
  compressed_matrix = blaz_compress(matrix);

  printf("Original matrix:\n");
  for(i=0; i<matrix->height; i++) {
    for(j=0; j<matrix->width; j++) {
      printf("%f ", blaz_get_matrix_elt(matrix, j, i));
    }
    printf("\n");
  }

  blaz_set_matrix_elt(matrix, -0.05, 1, 2);

  printf("Modified matrix:\n");
  for(i=0; i<matrix->height; i++) {
    for(j=0; j<matrix->width; j++) {
      printf("%f ", blaz_get_matrix_elt(matrix, j, i));
    }
    printf("\n");
  }

  printf("Compressed matrix:\n");
  for(i=0; i<matrix->height; i++) {
    for(j=0; j<matrix->width; j++) {
      printf("%f ", blaz_get_compressed_matrix_elt(compressed_matrix, j, i));
    }
    printf("\n");
  }

  blaz_set_compressed_matrix_elt(compressed_matrix, blaz_get_matrix_elt(matrix, 1, 2) * 1.5, 1, 2);

  printf("Modified compressed matrix:\n");
  for(i=0; i<matrix->height; i++) {
    for(j=0; j<matrix->width; j++) {
      printf("%f ", blaz_get_compressed_matrix_elt(compressed_matrix, j, i));
    }
    printf("\n");
  }
}
