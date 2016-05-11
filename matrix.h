#ifndef MATRIX_H
#define MATRIX_H
#include <stddef.h>
#include <stdint.h>

typedef struct vector_struct {
	double x;
	double y;
	double z;
} vector;

vector new_vector(double x, double y, double z);

typedef double *matrix;

void print_matrix(matrix m);

matrix new_matrix();

matrix identity_matrix();

void scale(double sx, double sy, double sz, matrix m);

void translate(double i, double j, double k, matrix m);

void rotate_x(double theta, matrix m);

void rotate_y(double theta, matrix m);

void rotate_z(double theta, matrix m);

matrix display_matrix(vector center, vector eye);
#endif
