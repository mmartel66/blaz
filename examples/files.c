/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : files.c
 * @Role ........ : IO: read, compress, write, read again,
                    uncompress a matrix. File name given
                    as argument in the command line (argv[1]).
                    Comparison of the original and
                    uncompressed matrices.
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/08/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <blaz.h>


int main(int argc, char **argv) {
  int i, j;
  Blaz_Matrix *matrix, *uncompressed_matrix;
  Blaz_Compressed_Matrix *compressed_matrix_1, *compressed_matrix_2;

  matrix = blaz_read_matrix(argv[1]);

  compressed_matrix_1 = blaz_compress(matrix);

  blaz_write_compressed_matrix(argv[1], compressed_matrix_1);

  compressed_matrix_2 = blaz_read_compressed_matrix(argv[1]);

  uncompressed_matrix = blaz_uncompress(compressed_matrix_2);

  for(i=0; i<uncompressed_matrix->height; i++) {
    for(j=0; j<uncompressed_matrix->width; j++) {
      printf("%f %f %f\n", 0.1*j, 0.1*i, uncompressed_matrix->matrix[POS(j, i, uncompressed_matrix->width)]);
    }
  }
}
