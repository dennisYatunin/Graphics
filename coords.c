#include "coords.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

matrix new_matrix() {
	matrix m = (matrix) malloc(16 * sizeof(double));
	if (m == NULL) {
		perror("New Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
	return m;
}

matrix mat_mat_multiply(matrix m1, matrix m2) {
	matrix m = new_matrix(), m_cpy = m, m1_cpy = m1;
	double a, b, c, d;
	int i = 0;
	while (i < 16) {
		a = *m1++; b = *m1++; c = *m1++; d = *m1++;
		*m++ = a * m2[0] + b * m2[4] + c * m2[8]  + d * m2[12];
		*m++ = a * m2[1] + b * m2[5] + c * m2[9]  + d * m2[13];
		*m++ = a * m2[2] + b * m2[6] + c * m2[10] + d * m2[14];
		*m++ = a * m2[3] + b * m2[7] + c * m2[11] + d * m2[15];
		i += 4;
	}
	free(m1_cpy);
	return m_cpy;
}

matrix identity_matrix() {
	matrix m = new_matrix(), m_cpy = m;
	*m++ = 1; *m++ = 0; *m++ = 0; *m++ = 0;
	*m++ = 0; *m++ = 1; *m++ = 0; *m++ = 0;
	*m++ = 0; *m++ = 0; *m++ = 1; *m++ = 0;
	*m++ = 0; *m++ = 0; *m++ = 0; *m   = 1;
	return m_cpy;
}

matrix scale_matrix(double sx, double sy, double sz) {
	matrix m = new_matrix(), m_cpy = m;
	*m++ = sx; *m++ =  0; *m++ =  0; *m++ = 0;
	*m++ =  0; *m++ = sy; *m++ =  0; *m++ = 0;
	*m++ =  0; *m++ =  0; *m++ = sz; *m++ = 0;
	*m++ =  0; *m++ =  0; *m++ =  0; *m   = 1;
	return m_cpy;
}

void scale(double sx, double sy, double sz, matrix m) {
	matrix temp = mat_mat_multiply(scale_matrix(sx, sy, sz), m);
	memcpy(m, temp, 16 * sizeof(double));
	free(temp);
}

matrix translation_matrix(double i, double j, double k) {
	matrix m = new_matrix(), m_cpy = m;
	*m++ = 1; *m++ = 0; *m++ = 0; *m++ = i;
	*m++ = 0; *m++ = 1; *m++ = 0; *m++ = j;
	*m++ = 0; *m++ = 0; *m++ = 1; *m++ = k;
	*m++ = 0; *m++ = 0; *m++ = 0; *m   = 1;
	return m_cpy;
}

void translate(double i, double j, double k, matrix m) {
	matrix temp = mat_mat_multiply(translation_matrix(i, j, k), m);
	memcpy(m, temp, 16 * sizeof(double));
	free(temp);
}

matrix rotation_matrix_x(double theta) {
	theta *= M_PI / 180;
	double c = cos(theta), s = sin(theta);
	matrix m = new_matrix(), m_cpy = m;
	*m++ = 1; *m++ =  0; *m++ = 0; *m++ = 0;
	*m++ = 0; *m++ =  c; *m++ = s; *m++ = 0;
	*m++ = 0; *m++ = -s; *m++ = c; *m++ = 0;
	*m++ = 0; *m++ =  0; *m++ = 0; *m   = 1;
	return m_cpy;
}

void rotate_x(double theta, matrix m) {
	matrix temp = mat_mat_multiply(rotation_matrix_x(theta), m);
	memcpy(m, temp, 16 * sizeof(double));
	free(temp);
}

matrix rotation_matrix_y(double theta) {
	theta *= M_PI / 180;
	double c = cos(theta), s = sin(theta);
	matrix m = new_matrix(), m_cpy = m;
	*m++ =  c; *m++ = 0; *m++ = s; *m++ = 0;
	*m++ =  0; *m++ = 1; *m++ = 0; *m++ = 0;
	*m++ = -s; *m++ = 0; *m++ = c; *m++ = 0;
	*m++ =  0; *m++ = 0; *m++ = 0; *m   = 1;
	return m_cpy;
}

void rotate_y(double theta, matrix m) {
	matrix temp = mat_mat_multiply(rotation_matrix_y(theta), m);
	memcpy(m, temp, 16 * sizeof(double));
	free(temp);
}

matrix rotation_matrix_z(double theta) {
	theta *= M_PI / 180;
	double c = cos(theta), s = sin(theta);
	matrix m = new_matrix(), m_cpy = m;
	*m++ =  c; *m++ = s; *m++ = 0; *m++ = 0;
	*m++ = -s; *m++ = c; *m++ = 0; *m++ = 0;
	*m++ =  0; *m++ = 0; *m++ = 1; *m++ = 0;
	*m++ =  0; *m++ = 0; *m++ = 0; *m   = 1;
	return m_cpy;
}

void rotate_z(double theta, matrix m) {
	matrix temp = mat_mat_multiply(rotation_matrix_z(theta), m);
	memcpy(m, temp, 16 * sizeof(double));
	free(temp);
}

matrix perp_matrix(vector center, vector view, double distance) {
	if (distance == 0) {
		fprintf(stderr, "Perp Matrix error: distance must be nonzero\n");
	}

	double square_xy = view.x * view.x + view.y * view.y;
	double radius_xyz = sqrt(square_xy + view.z * view.z);
	if (radius_xyz == 0) {
		fprintf(
			stderr, "Perp Matrix error: view vector must be nonzero\n"
			);
	}

	double radius_xy, sin_1, cos_1;
	if (square_xy) {
		radius_xy = sqrt(square_xy);
		sin_1 = view.x / radius_xy;
		cos_1 = view.y / radius_xy;
	}
	else { // view vector points along z-axis
		radius_xy = 0;
		sin_1 = 0;
		cos_1 = 1;
	}

	double sin_2 = view.z / radius_xyz;
	double cos_2 = radius_xy / radius_xyz;

	// pre-computed matrix for:
	// scale(
	// 	1, distance, 1, rotate_x(
	// 		atan(sin_2/cos_2),
	// 		rotate_z(
	// 			atan(sin_1/cos_1),
	// 			translate(
	// 				-center.x,-center.y,-center.z,
	// 				identity_matrix()
	// 				)
	// 			)
	// 		)
	// 	)

	matrix m = new_matrix(), m_cpy = m;
	*m++ = cos_1;
	*m++ = -sin_1;
	*m++ = 0;
	*m++ = -cos_1 * center.x + sin_1 * center.y;
	*m++ = distance * cos_2 * sin_1;
	*m++ = distance * cos_2 * cos_1;
	*m++ = distance * sin_2;
	*m++ = distance * (
		-cos_2 * sin_1 * center.x - cos_2 * cos_1 * center.y - sin_2 * center.z
		);
	*m++ = -sin_2 * sin_1;
	*m++ = -sin_2 * cos_1;
	*m++ = cos_2;
	*m++ =
	sin_2 * sin_1 * center.x + sin_2 * cos_1 * center.y - cos_2 * center.z;
	*m++ = 0; *m++ = 0; *m++ = 0; *m   = 1;
	return m_cpy;
}

stack new_stack(size_t max_num_matrices) {
	stack st = (stack) malloc(sizeof(struct stack_struct));
	if (st == NULL) {
		perror("New Stack error (malloc)");
		exit(EXIT_FAILURE);
	}
	st->size = 0;
	st->capacity = max_num_matrices;
	st->matrices = (matrix *) malloc(max_num_matrices * sizeof(matrix));
	if (st->matrices == NULL) {
		perror("New Stack error (malloc)");
		exit(EXIT_FAILURE);
	}
	st->matrices[st->size++] = identity_matrix();
	return st;
}

void clear_stack(stack st) {
	int counter = st->size;
	while (counter > 1) {
		free(st->matrices[counter--]);
	}
	st->size = 1;
}

void push(stack st) {
	if (st->size == st->capacity) {
		st->capacity *= 2;
		st->matrices = realloc(st->matrices, st->capacity * sizeof(matrix));
		if (st->matrices == NULL) {
			perror("Push error (realloc)");
			exit(EXIT_FAILURE);
		}
	}
	matrix m = new_matrix();
	memcpy(m, st->matrices[st->size - 1], 16 * sizeof(double));
	st->matrices[st->size++] = m;
}

void pop(stack st) {
	if (st->size == 1) {
		fprintf(stderr, "Pop error: cannot pop primary matrix\n");
		exit(EXIT_FAILURE);
	}
	free(st->matrices[--st->size]);
}

matrix peek(stack st) {
	return st->matrices[st->size - 1];
}
