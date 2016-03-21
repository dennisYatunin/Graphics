#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>

matrix make_matrix(size_t rows, size_t cols) {
	if ((uint64_t)rows * cols > SIZE_MAX) {
		fprintf(
			stderr,
			"Matrix Error: number of elements exceeds SIZE_MAX\n"
			);
		exit(EXIT_FAILURE);
	}
	matrix m = (matrix) malloc(sizeof(struct matrix_struct));
	if (m == NULL) {
		perror("memory allocation");
		exit(EXIT_FAILURE);
	}
	m->rows = rows;
	m->cols = cols;
	m->data = (double *) calloc(rows, cols * sizeof(double));
	if (m == NULL) {
		perror("memory allocation");
		exit(EXIT_FAILURE);
	}
	return m;
}

matrix identity_matrix(size_t rows) {
	matrix result = make_matrix(rows, rows);
	double *data = result->data;
	*data = 1;
	while (--rows) {
		data += rows + 1;
		*data = 1;
	}
	return result;
}

matrix translation_matrix(double i, double j, double k) {
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = 1;
	data[3] = i;
	data[5] = 1;
	data[7] = j;
	data[10] = 1;
	data[11] = k;
	data[15] = 1;
	return result;
}

matrix scale_matrix(double sx, double sy, double sz) {
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = sx;
	data[5] = sy;
	data[10] = sz;
	data[15] = 1;
	return result;
}

matrix rotation_matrix_x(double theta) {
	theta *= PI / 180;
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = 1;
	data[5] = cos(theta);
	data[6] = sin(theta);
	data[9] = -sin(theta);
	data[10] = cos(theta);
	data[15] = 1;
	return result;
}

matrix rotation_matrix_y(double theta) {
	theta *= PI / 180;
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = cos(theta);
	data[2] = sin(theta);
	data[5] = 1;
	data[8] = -sin(theta);
	data[10] = cos(theta);
	data[15] = 1;
	return result;
}

matrix rotation_matrix_z(double theta) {
	theta *= PI / 180;
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = cos(theta);
	data[1] = sin(theta);
	data[4] = -sin(theta);
	data[5] = cos(theta);
	data[10] = 1;
	data[15] = 1;
	return result;
}

void matrix_insert(matrix m, size_t row, size_t col, double value) {
	if (row >= m->rows || col >= m->cols) {
		fprintf(stderr, "Matrix Error: index out of bounds\n");
		exit(EXIT_FAILURE);
	}
	m->data[row * m->cols + col] = value;
}

double matrix_get(matrix m, size_t row, size_t col) {
	if (row >= m->rows || col >= m->cols) {
		fprintf(stderr, "Matrix Error: index out of bounds\n");
		exit(EXIT_FAILURE);
	}
	return m->data[row * m->cols + col];
}

void matrix_print(matrix m) {
	size_t rows = m->rows, cols = m->cols;
	double *data = m->data;
	size_t row = 0, col;
	while (row++ < rows) {
		col = 0;
		printf("| ");
		while (col++ < cols) {
			printf("%.2f\t", *data++);
		}
		printf("|\n");
	}
	printf("\n");
}

void matrix_multiply(matrix a, matrix b) {
	if (a->cols != b->rows) {
		fprintf(stderr, "Matrix Error: row-column mismatch\n");
		exit(EXIT_FAILURE);
	}
	double *dataa = a->data, *datab = b->data;
	size_t rowsa = a->rows, colsa = a->cols, colsb = b->cols;
	double *result = (double *) malloc(rowsa * colsb * sizeof(double));
	if (result == NULL) {
		perror("memory allocation");
		exit(EXIT_FAILURE);
	}
	size_t rowa, cola, colb;
	double sum;
	for (rowa = 0; rowa < rowsa; rowa++) {
		for (colb = 0; colb < colsb; colb++) {
			sum = 0;
			for (cola = 0; cola < colsa; cola++) {
				sum +=
				dataa[rowa * colsa + cola] * datab[cola * colsb + colb];
			}
			result[rowa * colsb + colb] = sum;
		}
	}
	free(datab);
	b->data = result;
	b->rows = rowsa;
	free(dataa);
	free(a);
}

matrix bezier_coef_matrix() {
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = -1;
	data[1] = 3;
	data[2] = -3;
	data[3] = 1;
	data[4] = 3;
	data[5] = -6;
	data[6] = 3;
	data[8] = -3;
	data[9] = 3;
	data[12] = 1;
	return result;
}

matrix hermite_coef_matrix() {
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = 2;
	data[1] = -2;
	data[2] = 1;
	data[3] = 1;
	data[4] = -3;
	data[5] = 3;
	data[6] = -2;
	data[7] = -1;
	data[10] = 1;
	data[12] = 1;
	return result;
}

matrix curve_coefs(double p1x, double p1y, double p2x, double p2y, int type) {
	matrix result = make_matrix(4, 1);
	double *data = result->data;
	data[0] = p1x;
	data[1] = p1y;
	data[2] = p2x;
	data[3] = p2y;
	switch (type) {
		case BEZIER:
			matrix_multiply(bezier_coef_matrix(), result);
			break;
		case HERMITE:
			matrix_multiply(hermite_coef_matrix(), result);
			break;
		default:
			fprintf(stderr, "Curve Error: Unknown curve type\n");
			exit(EXIT_FAILURE);
	}
	return result;
}
