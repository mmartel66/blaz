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
  int width, height, return_value;
  Blaz_Matrix *matrix;

  file = fopen(file_name,"r");
  if (file == NULL) read_error(file_name);

  return_value = fread(&width, sizeof(int), 1, file);
  if (return_value == 0) read_error(file_name);
  return_value = fread(&height, sizeof(int), 1, file);
  if (return_value == 0) read_error(file_name);

  matrix = (Blaz_Matrix*)blaz_malloc(sizeof(Blaz_Matrix));
  matrix->width = width;
  matrix->height = height;
  matrix->matrix = (double*)blaz_malloc(width * height * sizeof(double));

  return_value = fread(matrix->matrix,sizeof(double), width * height, file);
  if (return_value == 0) read_error(file_name);

  fclose(file);

  return matrix;
}


void blaz_write_matrix(char *file_name, Blaz_Matrix *matrix) {
  FILE *file;
  int return_value;

  file = fopen(file_name,"w");
  if (file == NULL) write_error(file_name);

  return_value = fwrite(&matrix->width, sizeof(int), 1, file);
  if (return_value == 0) write_error(file_name);
  return_value = fwrite(&matrix->height, sizeof(int), 1, file);
  if (return_value == 0) write_error(file_name);
  return_value = fwrite(matrix->matrix , sizeof(float), matrix->width * matrix->height, file);
  if (return_value == 0) write_error(file_name);

  fclose(file);
}


Blaz_Compressed_Matrix *blaz_read_compressed_matrix(char *file_name) {
  Blaz_Compressed_Matrix *compressed_matrix;
  FILE *file;
  char *compressed_file_name;
  int width, height, return_value;

  compressed_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));
  compressed_file_name = (char*)blaz_malloc((strlen(file_name)+7)*sizeof(char));
  compressed_file_name[0] = '\0';
  strcat(compressed_file_name,file_name);
  strcat(compressed_file_name,".blaz");
  compressed_file_name[strlen(file_name)+6] = '\0';

  file = fopen(compressed_file_name,"r");
  if (file == NULL) read_error(file_name);

  return_value = fread(&width, sizeof(int), 1, file);
  if (return_value == 0) read_error(file_name);
  return_value = fread(&height, sizeof(int), 1, file);
  if (return_value == 0) read_error(file_name);

  compressed_matrix = (Blaz_Compressed_Matrix*)blaz_malloc(sizeof(Blaz_Compressed_Matrix));
  compressed_matrix->width = width;
  compressed_matrix->height = height;

  compressed_matrix->block_first_elts = (double*)blaz_malloc(width * height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  compressed_matrix->block_mean_slope = (double*)blaz_malloc(width * height / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(double));
  compressed_matrix->compressed_values = (s_8*)blaz_malloc(width * height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE) * sizeof(s_8));

  return_value = fread(compressed_matrix->block_first_elts , sizeof(double), width * height / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) read_error(file_name);
  return_value = fread(compressed_matrix->block_mean_slope , sizeof(double), width * height / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) read_error(file_name);
  return_value = fread(compressed_matrix->compressed_values , sizeof(s_8), width * height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) read_error(file_name);

  fclose(file);

  return compressed_matrix;
}


void blaz_write_compressed_matrix(char *file_name, Blaz_Compressed_Matrix *compressed_matrix) {
  FILE *file;
  char *compressed_file_name;
  int return_value;

  compressed_file_name = blaz_malloc((strlen(file_name)+7)*sizeof(char));
  compressed_file_name[0] = '\0';
  strcat(compressed_file_name,file_name);
  strcat(compressed_file_name,".blaz");
  compressed_file_name[strlen(file_name)+6] = '\0';

  file = fopen(compressed_file_name,"w");
  if (file == NULL) read_error(file_name);

  return_value = fwrite(&compressed_matrix->width, sizeof(int), 1, file);
  if (return_value == 0) write_error(file_name);
  return_value = fwrite(&compressed_matrix->height, sizeof(int), 1, file);
  if (return_value == 0) write_error(file_name);

  return_value = fwrite(compressed_matrix->block_first_elts , sizeof(double), compressed_matrix->width * compressed_matrix->height / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) write_error(file_name);
  return_value = fwrite(compressed_matrix->block_mean_slope , sizeof(double), compressed_matrix->width * compressed_matrix->height / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) write_error(file_name);
  return_value = fwrite(compressed_matrix->compressed_values , sizeof(s_8), compressed_matrix->width * compressed_matrix->height * COMPRESSED_VECTOR_SIZE / (BLOCK_SIZE * BLOCK_SIZE), file);
  if (return_value == 0) write_error(file_name);

  fclose(file);
}
