

// IO: read, compress, write, read again, uncompress a matrix.
// File name given as argument in the command line (argv[1]).
// Comparison of the original and uncompressed matrices.


#include <stdio.h>
#include <blaz.h>


int main(int argc, char **argv) {
  int i, j;
  Blaz_Matrix *matrix, *uncompressed_matrix;
  Blaz_Compressed_Matrix *compressed_matrix_1, *compressed_matrix_2;

  matrix = read_blaz_matrix(argv[1]);

  compressed_matrix_1 = blaz_compress(matrix);

  write_blaz_compressed_matrix(argv[1], compressed_matrix_1);

  compressed_matrix_2 = read_blaz_compressed_matrix(argv[1]);

  uncompressed_matrix = blaz_uncompress(compressed_matrix_2);

  for(i=0; i<uncompressed_matrix->height; i++) {
    for(j=0; j<uncompressed_matrix->width; j++) {
      if(uncompressed_matrix->matrix[POS(j, i, uncompressed_matrix->width)] != matrix->matrix[POS(j, i, uncompressed_matrix->width)]) {
        printf("(%d,%d): %f %f\n", j, i, matrix->matrix[POS(j, i, uncompressed_matrix->width)], uncompressed_matrix->matrix[POS(j, i, uncompressed_matrix->width)]);
      }
    }
  }
}
