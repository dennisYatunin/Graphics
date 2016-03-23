#ifndef DRAW_H
#define DRAW_H
#include "matrix.h"
#include "screen.h"

typedef struct point_matrix_struct {
	size_t size;
	matrix points;
	uint32_t *colors;
} *point_matrix;

point_matrix make_point_matrix(size_t capacity);

void add_point(point_matrix pm, double x, double y, double z);

void add_edge(
	point_matrix pm,
	double x0, double y0, double z0,
	double x1, double y1, double z1,
	uint32_t color
	);

void add_circle(
	point_matrix pm,
	double cx, double cy, double r,
	int steps, uint32_t color
	);

#define BEZIER 0
#define HERMIT 1
void add_curve(
	point_matrix pm,
	double x0, double y0,
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	int steps, char type, uint32_t color
	);

void draw_line(
	screen s,
	uint32_t x0, uint32_t y0,
	uint32_t x1, uint32_t y1,
	uint32_t color
	);

void draw_lines(point_matrix pm, screen s);

#endif
