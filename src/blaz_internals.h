/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : blaz_internals.h
 * @Role ........ : interface for internal functions
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>


typedef signed short s_8;

#define POS(X,Y,WIDTH) (X + (Y) * WIDTH)

#define BLOCK_SIZE 8

#define COMPRESSED_VECTOR_SIZE 29


void *blaz_malloc(int);


double block_delta(double*, double*, int, int, int);

void block_undelta(double*, double*, double, int, int, int);


double block_slope(double*, double*);

void block_unslope(double*, double*, double);


void dct(double*, s_8*, int);

void idct(double*, s_8*, int);

void quantize(double*, float*, int, int);

void unquantize(float*, double*, int, int, int);


void read_error(char*);

void write_error(char*);
