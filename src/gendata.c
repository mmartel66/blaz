
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 16

int main() {
  int i,j;
  double x,y,*z;
  FILE *f;
  int n = N;
  double d;

  f = fopen("aaa8","w");

 int a = fwrite(&n,sizeof(int),1,f);
 int b = fwrite(&n,sizeof(int),1,f);
 printf("%d %d\n",a,b);

  z = (double*)malloc(N*N*sizeof(double));
  for(i=0;i<N;i++) {
    for(j=0;j<N;j++) {
      x = 0.1 * (double)j;
      y = 0.1 * (double)i;
      z[i*N+j] = d = x * x- y * y;
      printf("%f %f %f\n",x,y,d);
      fwrite(&d,sizeof(double),1,f);
    }
  }
  //int t = fwrite(z,sizeof(double),N*N,f);
  //printf("mmmm %d\n",t);
  fclose(f);
}
