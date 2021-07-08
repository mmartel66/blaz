/*******************************************************
 * This file is part of the Blaz library
 * @Name ........ : io.c
 * @Role ........ : functions to read/write matrices in
                    files
 * @Author ...... : Matthieu Martel
 * @Version ..... : V1.1 06/30/2021
 * @Licence ..... : GPL V3
 * @Link ........ : https://github.com/mmartel66/blaz.git
 ********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <blaz.h>

Blaz_Matrix *blaz_read_matrix(char *file_name) {
  FILE *file;
  int width, height;
  Blaz_Matrix *matrix;

  file = fopen(file_name,"r");

  fread(&width, sizeof(int), 1, file);
  fread(&height, sizeof(int), 1, file);

  matrix = (Blaz_Matrix*)malloc(sizeof(Blaz_Matrix));
  matrix->width = width;
  matrix->height = height;
  matrix->matrix = (double*)malloc(width * height * sizeof(double));

  fread(matrix->matrix,sizeof(double), width * height, file);
  fclose(file);

  return matrix;
}


void blaz_write_matrix(char *file_name, Blaz_Matrix *matrix) {
  FILE *file;

  file = fopen(file_name,"w");

  fwrite(&matrix->width, sizeof(int), 1, file);
  fwrite(&matrix->height, sizeof(int), 1, file);
  fwrite(matrix->matrix , sizeof(float), matrix->width * matrix->height, file);

  fclose(file);
}


Blaz_Compressed_Matrix *blaz_read_compressed_matrix(char *file_name) {
  Blaz_Compressed_Matrix *compressed_matrix;
  FILE *file;
  char *compressed_file_name;
  int width, height;

  compressed_matrix = (Blaz_Compressed_Matrix*)malloc(sizeof(Blaz_Compressed_Matrix));
  compressed_file_name = (char*)malloc((strlen(file_name)+7)*sizeof(char));
  compressed_file_name[0] = '\0';
  strcat(compressed_file_name,file_name);
  strcat(compressed_file_name,".blaz");
  compressed_file_name[strlen(file_name)+6] = '\0';

  file = fopen(compressed_file_name,"r");

  fread(&width, sizeof(int), 1, file);
  fread(&height, sizeof(int), 1, file);

  compressed_matrix = (Blaz_Compressed_Matrix*)malloc(sizeof(Blaz_Compressed_Matrix));
  compressed_matrix->width = width;
  compressed_matrix->height = height;

  compressed_matrix->block_first_elts = (double*)malloc(width * height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  compressed_matrix->block_mean_slope = (double*)malloc(width * height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  compressed_matrix->compressed_values = (s_8*)malloc(width * height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

  fread(compressed_matrix->block_first_elts , sizeof(double), width * height / (BLOCK_SIZE * BLOCK_SIZE), file);
  fread(compressed_matrix->block_mean_slope , sizeof(double), width * height / (BLOCK_SIZE * BLOCK_SIZE), file);
  fread(compressed_matrix->compressed_values , sizeof(s_8), width * height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE), file);
  fclose(file);

  return compressed_matrix;
}


void blaz_write_compressed_matrix(char *file_name, Blaz_Compressed_Matrix *compressed_matrix) {
  FILE *file;
  char *compressed_file_name;

  compressed_file_name = malloc((strlen(file_name)+7)*sizeof(char));
  compressed_file_name[0] = '\0';
  strcat(compressed_file_name,file_name);
  strcat(compressed_file_name,".blaz");
  compressed_file_name[strlen(file_name)+6] = '\0';

  file = fopen(compressed_file_name,"w");

  fwrite(&compressed_matrix->width, sizeof(int), 1, file);
  fwrite(&compressed_matrix->height, sizeof(int), 1, file);

  fwrite(compressed_matrix->block_first_elts , sizeof(double), compressed_matrix->width * compressed_matrix->height / (BLOCK_SIZE * BLOCK_SIZE), file);
  fwrite(compressed_matrix->block_mean_slope , sizeof(double), compressed_matrix->width * compressed_matrix->height / (BLOCK_SIZE * BLOCK_SIZE), file);
  fwrite(compressed_matrix->compressed_values , sizeof(s_8), compressed_matrix->width * compressed_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE), file);

  fclose(file);
}
