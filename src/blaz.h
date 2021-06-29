

#include<blaz_internals.h>


typedef struct {
  int width, height;
  double *matrix;
} Blaz_Matrix;


typedef struct {
  int width, height;
  double *block_first_elts, *block_mean_slope;
  s_8 *compressed_values;
} Blaz_Compressed_Matrix;


Blaz_Matrix *read_blaz_matrix(char*);

void write_blaz_matrix(char *, Blaz_Matrix*);


Blaz_Compressed_Matrix *read_blaz_compressed_matrix(char *);

void write_blaz_compressed_matrix(char*, Blaz_Compressed_Matrix*);


Blaz_Compressed_Matrix *blaz_compress(Blaz_Matrix*);

Blaz_Matrix* blaz_uncompress(Blaz_Compressed_Matrix*);



Blaz_Matrix *add(Blaz_Matrix*, Blaz_Matrix*);

Blaz_Compressed_Matrix *add_compressed(Blaz_Compressed_Matrix*, Blaz_Compressed_Matrix*);
