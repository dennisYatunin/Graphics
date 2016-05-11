#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

vector new_vector(double x, double y, double z) {
	vector result;
	result.x = x; result.y = y; result.z = z;
	return result;
}

void print_matrix(matrix m) {
	printf(
		"-                           -\n"
		"|%6.2lf %6.2lf %6.2lf %6.2lf|\n"
		"|%6.2lf %6.2lf %6.2lf %6.2lf|\n"
		"|%6.2lf %6.2lf %6.2lf %6.2lf|\n"
		"|%6.2lf %6.2lf %6.2lf %6.2lf|\n"
		"-                           -\n",
		m[0],  m[1],  m[2],  m[3],
		m[4],  m[5],  m[6],  m[7],
		m[8],  m[9],  m[10], m[11],
		m[12], m[13], m[14], m[15]
		);
}

matrix new_matrix() {
	matrix m = (matrix) malloc(16 * sizeof(double));
	if (m == NULL) {
		perror("New Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
	return m;
}

matrix mat_mat_multiply(matrix m1, matrix m2) {
	matrix m = new_matrix(), m_cpy = m;
	double a, b, c, d;
	int row = 0;
	while (row++ < 4) {
		a = *m2++; b = *m2++; c = *m2++; d = *m2++;
		*m++ = a * m1[0] + b * m1[4] + c * m1[8]  + d * m1[12];
		*m++ = a * m1[1] + b * m1[5] + c * m1[9]  + d * m1[13];
		*m++ = a * m1[2] + b * m1[6] + c * m1[10] + d * m1[14];
		*m++ = a * m1[3] + b * m1[7] + c * m1[11] + d * m1[15];
	}
	free(m1);
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

matrix display_matrix(vector center, vector eye) {
	double etoc_x = center.x - eye.x,
	etoc_y = center.y - eye.y,
	etoc_z = center.z - eye.z;

	if (etoc_x == 0 && etoc_y == 0 && etoc_z == 0) {
		fprintf(
			stderr, "Display Matrix error: "
			"center and eye vectors must be different\n"
			);
	}

	double square_xy = etoc_x * etoc_x + etoc_y * etoc_y;

	/*
		Determine sin_1 and cos_1 such that, when the matrix
		mat_mat_multiply(
			rotation_matrix_z(atan2(sin_1, cos_1) * 180 / M_PI),
			translation_matrix(-eye.x, -eye.y, -eye.z)
			)
		is multiplied by the center vector, the new vector lies in the yz-plane.
	*/
	double radius_xy, sin_1, cos_1;
	if (square_xy) {
		radius_xy = sqrt(square_xy);
		cos_1 = etoc_y / radius_xy;
		sin_1 = etoc_x / radius_xy;
	}
	else {
		// etoc vector points along the z-axis, so atan2(sin_1, cos_1) is
		// arbitrarily made equal to 0 (thus avoiding a division by 0)
		radius_xy = 0;
		cos_1 = 1;
		sin_1 = 0;
	}

	/*
		Determine sin_2 and cos_2 such that, when the matrix
		mat_mat_multiply(
			rotation_matrix_x(atan2(sin_2, cos_2) * 180 / M_PI),
			mat_mat_multiply(
				rotation_matrix_z(atan2(sin_1, cos_1) * 180 / M_PI),
				translation_matrix(-eye.x, -eye.y, -eye.z)
				)
			)
		is multiplied by the center vector, the new vector is antiparallel to
		the z-axis.
	*/
	double radius_xyz = sqrt(square_xy + etoc_z * etoc_z);
	double cos_2 = -etoc_z / radius_xyz;
	double sin_2 = radius_xy / radius_xyz;

	/*
		Let m be the matrix
		mat_mat_multiply(
			rotation_matrix_x(atan2(sin_2, cos_2) * 180 / M_PI),
			mat_mat_multiply(
				rotation_matrix_z(atan2(sin_1, cos_1) * 180 / M_PI),
				translation_matrix(-eye.x, -eye.y, -eye.z)
				)
			).

		This matrix will change the coordinate system so that the eye is at the
		origin and the vector toward the center is antiparallel to the z-axis.
		In this new system, the z-coord of a point is the negative of that
		point's "depth" (its distance from the eye).
	*/
	matrix m = new_matrix(), m_cpy = m;
	*m++ = cos_1;
	*m++ = sin_1;
	*m++ = 0;
	*m++ = -cos_1 * eye.x - sin_1 * eye.y;
	*m++ = -cos_2 * sin_1;
	*m++ = cos_2 * cos_1;
	*m++ = sin_2;
	*m++ = cos_2 * sin_1 * eye.x - cos_2 * cos_1 * eye.y - sin_2 * eye.z;
	*m++ = sin_2 * sin_1;
	*m++ = -sin_2 * cos_1;
	*m++ = cos_2;
	*m++ = -sin_2 * sin_1 * eye.x + sin_2 * cos_1 * eye.y - cos_2 * eye.z;
	*m++ = 0;
	*m++ = 0;
	*m++ = 0;
	*m++ = 1;
	return m_cpy;
}
