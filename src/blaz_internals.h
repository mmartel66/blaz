
typedef signed short s_8;
typedef unsigned short int u_16;
typedef unsigned long int u_64;


#define POS(X,Y,WIDTH) (X + (Y) * WIDTH)

#define BLOCK_SIZE 8

#define COMPRESSED_VECTOR_SIZE 29


double block_delta(double*, double*, int, int, int);

void block_undelta(double*, double*, double, int, int, int);


double block_slope(double*, double*);

void block_unslope(double*, double*, double);


void dct(double*, s_8*, int);

void idct(double*, s_8*, int);

void quantize(double*, float*, int, int);

void unquantize(float*, double*, int, int, int);
