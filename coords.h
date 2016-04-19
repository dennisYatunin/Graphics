#ifndef COORDS_H
#define COORDS_H
#include <stddef.h>
#include <stdint.h>

typedef double *matrix;

typedef struct stack_struct {
	size_t size;
	size_t capacity;
	matrix *matrices;
} *stack;

typedef struct vector_struct {
	double x;
	double y;
	double z;
} vector;

matrix new_matrix();

matrix identity_matrix();

void scale(double sx, double sy, double sz, matrix m);

void translate(double i, double j, double k, matrix m);

void rotate_x(double theta, matrix m);

void rotate_y(double theta, matrix m);

void rotate_z(double theta, matrix m);

matrix perp_matrix(vector center, vector view, double distance);

stack new_stack(size_t max_num_matrices);

void clear_stack(stack st);

void push(stack st);

void pop(stack st);

matrix peek(stack st);
#endif
