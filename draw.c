#include "draw.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

point_matrix make_point_matrix(size_t capacity) {
	point_matrix pm = (point_matrix)malloc(sizeof(struct point_matrix_struct));
	if (pm == NULL) {
		perror("memory allocation");
		exit(EXIT_FAILURE);
	}
	pm->size = 0;
	pm->points = make_matrix(4, capacity);
	return pm;
}

void add_point(point_matrix pm, double x, double y, double z) {
	size_t size = pm->size;
	size_t capacity = pm->points->cols;
	if (size == capacity) {
		fprintf(
			stderr,
			"Point Matrix Error: number of elements exceeds capacity\n"
			);
		exit(EXIT_FAILURE);
	}
	double *data = pm->points->data;
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

void add_circle(point_matrix pm, double cx, double cy, double r, double steps) {
	double t;
	double t_inc = 1 / steps;
	double t_max = 1 + t_inc / 2;
	double p1x = cx + r, p1y = cy, p2x, p2y;

	for (t = 0; t < t_max; t += t_inc) {
		p2x = cx + r * cos(2 * PI * t);
		p2y = cx + r * sin(2 * PI * t);

		add_edge(pm, p1x, p1y, 0, p2x, p2y, 0);

		p1x = p2x;
		p1y = p2y;
	}
}

void add_curve(
	point_matrix pm,
	double x0, double y0,
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double steps, int type
	) {
	switch (type) {
		case BEZIER:
			matrix x_coefs = curve_coefs(x0, x1, x2, x3, BEZIER);
			matrix y_coefs = curve_coefs(y0, y1, y2, y3, BEZIER);
			double t;
			double t_inc = 1 / steps;
			double t_max = 1 + t_inc / 2;
			for (t = 0; t < t_max; t += t_inc) {
				add_edge(
					pm,
					x_coefs->data[0]*t*t*t + x_coefs->data[1]*t*t +
					x_coefs->data[2]*t + x_coefs->data[3],
					y_coefs->data[0]*t*t*t + y_coefs->data[1]*t*t +
					y_coefs->data[2]*t + y_coefs->data[3],
					0,
					x_coefs->data[0]*t*t*t + x_coefs->data[1]*t*t +
					x_coefs->data[2]*t + x_coefs->data[3],
					y_coefs->data[0]*t*t*t + y_coefs->data[1]*t*t +
					y_coefs->data[2]*t + y_coefs->data[3],
					0
					);
			}
			break;
		case HERMITE:
			matrix x_coefs = curve_coefs(x0, x1, x2, x3, HERMITE);
			matrix y_coefs = curve_coefs(y0, y1, y2, y3, HERMITE);
			double t;
			double t_inc = 1 / steps;
			double t_max = 1 + t_inc / 2;
			for (t = 0; t < t_max; t += t_inc) {
				add_edge(
					pm,
					x_coefs->data[0]*t*t*t + x_coefs->data[1]*t*t +
					x_coefs->data[2]*t + x_coefs->data[3],
					y_coefs->data[0]*t*t*t + y_coefs->data[1]*t*t +
					y_coefs->data[2]*t + y_coefs->data[3],
					0,
					x_coefs->data[0]*t*t*t + x_coefs->data[1]*t*t +
					x_coefs->data[2]*t + x_coefs->data[3],
					y_coefs->data[0]*t*t*t + y_coefs->data[1]*t*t +
					y_coefs->data[2]*t + y_coefs->data[3],
					0
					);
			}
			break;
		default:
			fprintf(stderr, "Curve Error: Unknown curve type\n");
			exit(EXIT_FAILURE);
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

void draw_lines(point_matrix pm, screen s) {
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
			rgb(255, 255, 255)
			);
		counter += 2;
	}
}
