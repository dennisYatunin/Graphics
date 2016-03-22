#include "draw.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

point_matrix make_point_matrix(size_t capacity) {
	point_matrix pm = (point_matrix) malloc(sizeof(struct point_matrix_struct));
	if (pm == NULL) {
		perror("Point Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
	pm->size = 0;
	pm->points = make_matrix(4, capacity);
	return pm;
}

void add_point(point_matrix pm, double x, double y, double z) {
	size_t size = pm->size;
	size_t capacity = pm->points->cols;
	double *data = pm->points->data;
	if (size == capacity) {
		capacity *= 2;
		pm->points->cols = capacity;
		realloc(data, 4 * capacity * sizeof(double));
	}
	data[size] = x;
	data[capacity + size] = y;
	data[2 * capacity + size] = z;
	data[3 * capacity + size] = 1;
	pm->size = size + 1;
}

void add_edge(
	point_matrix pm,
	double x0, double y0, double z0,
	double x1, double y1, double z1
	) {
	add_point(pm, x0, y0, z0);
	add_point(pm, x1, y1, z1);
}

void add_circle(point_matrix pm, double cx, double cy, double r, int steps) {
	int step;
	double t = 0.0, t_inc = 1.0 / steps;
	double p1x = cx + r, p1y = cy, p2x, p2y;

	for (step = 0; step < steps; step++) {
		t += t_inc;

		p2x = cx + r * cos(2 * PI * t);
		p2y = cx + r * sin(2 * PI * t);

		add_edge(pm, p1x, p1y, 0, p2x, p2y, 0);

		p1x = p2x;
		p1y = p2y;
	}
}

void set_bezier_coefs(
	double *array, double p1, double p2, double p3, double p4
	) {
	matrix factors = make_matrix(4, 4);
	double *data = factors->data;
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
	matrix coefs = make_matrix(4, 1);
	data = coefs->data;
	data[0] = p1;
	data[1] = p2;
	data[2] = p3;
	data[3] = p4;
	matrix_multiply(factors, coefs);
	array[0] = data[0];
	array[1] = data[1];
	array[2] = data[2];
	array[3] = data[3];
	free(data);
	free(coefs);
}

void set_hermite_coefs(
	double *array, double p1, double r1, double p2, double r2
	) {
	matrix factors = make_matrix(4, 4);
	double *data = factors->data;
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
	matrix coefs = make_matrix(4, 1);
	data = coefs->data;
	data[0] = p1;
	data[1] = r1;
	data[2] = p2;
	data[3] = r2;
	matrix_multiply(factors, coefs);
	array[0] = data[0];
	array[1] = data[1];
	array[2] = data[2];
	array[3] = data[3];
	free(data);
	free(coefs);
}

void add_curve(
	point_matrix pm,
	double x0, double y0,
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	int steps, int type
	) {
	double x_coefs[4], y_coefs[4];

	switch (type) {
		case BEZIER:
			set_bezier_coefs(x_coefs, x0, x1, x2, x3);
			set_bezier_coefs(y_coefs, y0, y1, y2, y3);
			break;

		case HERMITE:
			set_hermite_coefs(x_coefs, x0, x1, x2, x3);
			set_hermite_coefs(y_coefs, y0, y1, y2, y3);
			break;

		default:
			fprintf(stderr, "Curve error: unknown curve type\n");
			exit(EXIT_FAILURE);
	}

	int step;
	double t = 0.0, t_inc = 1.0 / steps;
	double p1x = x_coefs[3], p1y = y_coefs[3], p2x, p2y;

	for (step = 0; step < steps; step++) {
		t += t_inc;

		p2x = ((x_coefs[0] * t + x_coefs[1]) * t + x_coefs[2]) * t + x_coefs[3];
		p2y = ((y_coefs[0] * t + y_coefs[1]) * t + y_coefs[2]) * t + y_coefs[3];

		add_edge(pm, p1x, p1y, 0, p2x, p2y, 0);

		p1x = p2x;
		p1y = p2y;
	}
}


void draw_line(
	screen s,
	uint32_t x0, uint32_t y0,
	uint32_t x1, uint32_t y1,
	uint32_t value
	) {
	int32_t dx = abs(x1 - x0);
	int32_t dy = abs(y1 - y0);

	int32_t x_step = x0 < x1 ? 1 : -1;
	int32_t y_step = y0 < y1 ? 1 : -1;

	int32_t error = (dx > dy ? dx : -dy) / 2;
	int32_t original_error;

	while (1) {
		plot(s, x0, y0, value);
		if (x0 == x1 && y0 == y1) {
			return;
		}
		original_error = error;
		if (original_error > -dx) {
			error -= dy;
			x0 += x_step;
		}
		if (original_error < dy) {
			error += dx;
			y0 += y_step;
		}
	}
}

void draw_lines(point_matrix pm, screen s, uint32_t value) {
	size_t size = pm->size;
	if (size % 2 == 1) {
		fprintf(
			stderr,
			"Draw Lines Error: point matrix has unpaired point\n"
			);
		exit(EXIT_FAILURE);
	}
	size_t capacity = pm->points->cols;
	double *data = pm->points->data;
	size_t counter = 0;
	while (counter < size) {
		draw_line(
			s,
			(uint32_t)(data[counter] + 0.5),
			(uint32_t)(data[capacity + counter] + 0.5),
			(uint32_t)(data[counter + 1] + 0.5),
			(uint32_t)(data[capacity + counter + 1] + 0.5),
			value
			);
		counter += 2;
	}
}
