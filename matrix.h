#ifndef MATRIX_H
#define MATRIX_H
#include <stdlib.h>

#define PI 3.14159265

typedef struct matrix_struct {
	size_t rows;
	size_t cols;
	double *data;
} *matrix;

matrix make_matrix(size_t rows, size_t cols);

matrix identity_matrix(size_t rows);

matrix translation_matrix(double i, double j, double k);

matrix scale_matrix(double sx, double sy, double sz);

matrix rotation_matrix_x(double theta);

matrix rotation_matrix_y(double theta);

matrix rotation_matrix_x(double theta);

void matrix_insert(matrix m, size_t row, size_t col, double value);

double matrix_get(matrix m, size_t row, size_t col);

void matrix_print(matrix m);

void matrix_multiply(matrix a, matrix b);

#endif