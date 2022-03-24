/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : generate_data.c
 * @Role ........ : generation of uncompressed data sets to 
 *                  test the blaz library
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 23/03/2022
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 128

int main() {
  int i, j;
  double x, y, *z, step;
  FILE *f;
  int n = N;
  double d;

  f = fopen("mydata.ubz","w");

  int a = fwrite(&n,sizeof(int),1,f);
  int b = fwrite(&n,sizeof(int),1,f);

  step = 4.0 / N;
  z = (double*)malloc(N*N*sizeof(double));
  y = -2.0;

  for(i=0;i<N;i++) {
    x = -2.0;
    for(j=0;j<N;j++) {
      d = cos(x*x +y*y)*exp(-0.1*(x*x+y*y)) ;
      fwrite(&d,sizeof(double),1,f);
      x += step;
    }
    y += step;
  }
  fclose(f);
}

