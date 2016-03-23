#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>

matrix make_matrix(size_t rows, size_t cols) {
	if ((uint64_t)rows * cols > SIZE_MAX) {
		fprintf(
			stderr,
			"Make Matrix error: number of elements exceeds SIZE_MAX\n"
			);
		exit(EXIT_FAILURE);
	}
	matrix m = (matrix) malloc(sizeof(struct matrix_struct));
	if (m == NULL) {
		perror("Make Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
	m->rows = rows;
	m->cols = cols;
	m->data = (double *) calloc(rows, cols * sizeof(double));
	if (m->data == NULL) {
		perror("Make Matrix error (calloc)");
		exit(EXIT_FAILURE);
	}
	return m;
}

void free_matrix(matrix m) {
	free(m->data);
	free(m);
}

double *expand_matrix(matrix m) {
	size_t rows = m->rows, cols = m->cols;
	double *old_data = m->data;
	double *new_data = (double *) calloc(rows, 2 * cols * sizeof(double));
	if (new_data == NULL) {
		perror("Expand Matrix error (calloc)");
		exit(EXIT_FAILURE);
	}
	double *new_data_copy = new_data;
	int cur_col;
	while (rows--) {
		for (cur_col = 0; cur_col < cols; cur_col++) {
			*new_data++ = *old_data++;
		}
		new_data += cols;
	}
	m->cols *= 2;
	free(m->data);
	m->data = new_data_copy;
	return new_data_copy;
}

void matrix_insert(matrix m, size_t row, size_t col, double value) {
	if (row >= m->rows || col >= m->cols) {
		fprintf(stderr, "Matrix Insert error: index out of bounds\n");
		exit(EXIT_FAILURE);
	}
	m->data[row * m->cols + col] = value;
}

double matrix_get(matrix m, size_t row, size_t col) {
	if (row >= m->rows || col >= m->cols) {
		fprintf(stderr, "Matrix Get error: index out of bounds\n");
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
		fprintf(stderr, "Matrix Multiply error: row-column mismatch\n");
		exit(EXIT_FAILURE);
	}
	double *dataa = a->data, *datab = b->data;
	size_t rowsa = a->rows, colsa = a->cols, colsb = b->cols;
	double *result = (double *) malloc(rowsa * colsb * sizeof(double));
	if (result == NULL) {
		perror("Matrix Multiply error (malloc)");
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

matrix scale_matrix(double sx, double sy, double sz) {
	matrix result = make_matrix(4, 4);
	double *data = result->data;
	data[0] = sx;
	data[5] = sy;
	data[10] = sz;
	data[15] = 1;
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
