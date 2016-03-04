#include "draw.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

point_matrix make_point_matrix(size_t capacity) {
	point_matrix pm = (point_matrix) malloc(sizeof(struct point_matrix_struct));
	if (pm == NULL) {
		perror("memory allocation");
		exit(EXIT_FAILURE);
	}
	pm->size = 0;
	pm->points = make_matrix(4, capacity);
	return pm;
}

void add_point(point_matrix pm, uint32_t x, uint32_t y, uint32_t z) {
	size_t size = pm->size;
	size_t capacity = pm->points->cols;
	if (size == capacity) {
		fprintf(stderr, "Point Matrix Error: num elements exceeds capacity\n");
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
	uint32_t x0, uint32_t y0, uint32_t z0,
	uint32_t x1, uint32_t y1, uint32_t z1
	) {
	add_point(pm, x0, y0, z0);
	add_point(pm, x1, y1, z1);
}

void draw_line(
	screen s,
	uint32_t x0, uint32_t y0,
	uint32_t x1, uint32_t y1,
	uint32_t value
	) {
	uint32_t dx = abs(x1 - x0);
	uint32_t dy = abs(y1 - y0);

	uint32_t x_step = x0 < x1 ? 1 : -1;
	uint32_t y_step = y0 < y1 ? 1 : -1;

	uint32_t error = (dx > dy ? dx : -dy) / 2;
	uint32_t original_error;

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
		fprintf(stderr, "Draw Lines Error: point matrix has unpaired point\n");
		exit(EXIT_FAILURE);
	}
	size_t capacity = pm->points->cols;
	double *data = pm->points->data;
	size_t counter = 0;
	while (counter < size) {
		draw_line(
			s,
			round(data[counter]), round(data[capacity + counter]),
			round(data[counter + 1]), round(data[capacity + counter + 1]),
			rgb(255, 255, 255)
			);
		counter += 2;
	}
}
