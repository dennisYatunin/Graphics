#ifndef DRAW_H
#define DRAW_H
#include "matrix.h"
#include "screen.h"

typedef struct point_matrix_struct {
	size_t size;
	matrix points;
} *point_matrix;

point_matrix make_point_matrix(size_t capacity);

void add_point(point_matrix pm, uint32_t x, uint32_t y, uint32_t z);

void add_edge(
	point_matrix pm,
	uint32_t x0, uint32_t y0, uint32_t z0,
	uint32_t x1, uint32_t y1, uint32_t z1
	);

void draw_line(
	screen s,
	uint32_t x0, uint32_t y0,
	uint32_t x1, uint32_t y1,
	uint32_t value
	);

void draw_lines(point_matrix pm, screen s);

#endif
