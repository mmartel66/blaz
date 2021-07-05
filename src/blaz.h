/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : blaz.h
 * @Role ........ : interface of the blaz library
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

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



Blaz_Matrix *blaz_add(Blaz_Matrix*, Blaz_Matrix*);

Blaz_Compressed_Matrix *blaz_add_compressed(Blaz_Compressed_Matrix*, Blaz_Compressed_Matrix*);


Blaz_Matrix *blaz_mul_cst(Blaz_Matrix*, double);

Blaz_Compressed_Matrix *blaz_mul_cst_compressed(Blaz_Compressed_Matrix*, double);


double blaz_dot_product(Blaz_Matrix*, Blaz_Matrix*, int, int);

double blaz_dot_product_compressed(Blaz_Compressed_Matrix*, Blaz_Compressed_Matrix*, int, int);
