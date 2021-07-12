/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : errors.c
 * @Role ........ : Functions to manage errors
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 07/08/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>


void *blaz_malloc(int size) {
  void * ptr;

  ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Blaz: Failed to allocate memory.\n");
    exit(0);
  }

  return ptr;
}


void read_error(char *file_name) {
  fprintf(stderr,"Blaz: Failed to read file %s\n",file_name);
  exit(0);
}


void write_error(char *file_name) {
  fprintf(stderr,"Blaz: Failed to write file %s\n",file_name);
  exit(0);
}
