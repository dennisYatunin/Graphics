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
	pm->colors = (uint32_t *) malloc(capacity * sizeof(uint32_t));
	if (pm->colors == NULL) {
		perror("Point Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
	return pm;
}

void clear_point_matrix(point_matrix pm) {
	size_t capacity = pm->points->cols;
	free(pm->points);
	free(pm->colors);
	pm->size = 0;
	pm->points = make_matrix(4, capacity);
	pm->colors = (uint32_t *) malloc(capacity * sizeof(uint32_t));
	if (pm->colors == NULL) {
		perror("Point Matrix error (malloc)");
		exit(EXIT_FAILURE);
	}
}

void draw_line(
	screen s,
	int32_t x0, int32_t y0,
	int32_t x1, int32_t y1,
	uint32_t color
	) {
	int32_t dx = abs(x1 - x0);
	int32_t dy = abs(y1 - y0);

	int32_t x_step = x0 < x1 ? 1 : -1;
	int32_t y_step = y0 < y1 ? 1 : -1;

	int32_t error = (dx > dy ? dx : -dy) / 2;
	int32_t original_error;

	while (1) {
		plot(s, x0, y0, color);
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
			"Draw Lines error: point matrix has unpaired point\n"
			);
		exit(EXIT_FAILURE);
	}
	size_t capacity = pm->points->cols;
	double *data = pm->points->data;
	uint32_t *colors = pm->colors;
	size_t counter = 0;
	while (counter < size) {
		draw_line(
			s,
			(int32_t)(data[counter] + 0.5),
			(int32_t)(data[capacity + counter] + 0.5),
			(int32_t)(data[counter + 1] + 0.5),
			(int32_t)(data[capacity + counter + 1] + 0.5),
			colors[counter]
			);
		counter += 2;
	}
}

void add_point(point_matrix pm, double x, double y, double z) {
	size_t size = pm->size;
	size_t capacity = pm->points->cols;
	if (size == capacity) {
		expand_matrix(pm->points);
		capacity = pm->points->cols;
		pm->colors =
		(uint32_t *) realloc(pm->colors, capacity * sizeof(uint32_t));
		if (pm->colors == NULL) {
			perror("Point Matrix error (realloc)");
			exit(EXIT_FAILURE);
		}
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
	double x1, double y1, double z1,
	uint32_t color
	) {
	add_point(pm, x0, y0, z0);
	add_point(pm, x1, y1, z1);
	pm->colors[pm->size - 2] = color;
}

void add_circle(
	point_matrix pm,
	double cx, double cy, double r,
	int steps, uint32_t color
	) {
	double t = 0.0, t_inc = 2 * M_PI / steps;
	double p1x = cx + r, p1y = cy, p2x, p2y;

	while (--steps) {
		t += t_inc;

		p2x = cx + r * cos(t);
		p2y = cy + r * sin(t);
		add_edge(pm, p1x, p1y, 0, p2x, p2y, 0, color);
		p1x = p2x;
		p1y = p2y;
	}

	p2x = cx + r;
	p2y = cy;
	add_edge(pm, p1x, p1y, 0, p2x, p2y, 0, color);
}

void add_curve(
	point_matrix pm,
	double x0, double y0,
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	int steps, char type, uint32_t color
	) {
	double a_x, b_x, c_x, d_x, a_y, b_y, c_y, d_y;

	switch (type) {
		case BEZIER:
			a_x = -1 * x0 + 3 * x1 - 3 * x2 + x3;
			a_y = -1 * y0 + 3 * y1 - 3 * y2 + y3;
			b_x =  3 * x0 - 6 * x1 + 3 * x2     ;
			b_y =  3 * y0 - 6 * y1 + 3 * y2     ;
			c_x = -3 * x0 + 3 * x1              ;
			c_y = -3 * y0 + 3 * y1              ;
			d_x =      x0                       ;
			d_y =      y0                       ;
			break;

		case HERMIT:
			a_x =  2 * x0 +     x1 - 2 * x2 + x3;
			a_y =  2 * y0 +     y1 - 2 * y2 + y3;
			b_x = -3 * x0 - 2 * x1 + 3 * x2 - x3;
			b_y = -3 * y0 - 2 * y1 + 3 * y2 - y3;
			c_x =               x1              ;
			c_y =               y1              ;
			d_x =      x0                       ;
			d_y =      y0                       ;
			break;

		default:
			fprintf(stderr, "Curve error: unknown curve type\n");
			exit(EXIT_FAILURE);
	}

	double t = 0.0, t_inc = 1.0 / steps;
	double p1x = d_x, p1y = d_y, p2x, p2y;

	while (--steps) {
		t += t_inc;

		p2x = ((a_x * t + b_x) * t + c_x) * t + d_x;
		p2y = ((a_y * t + b_y) * t + c_y) * t + d_y;
		add_edge(pm, p1x, p1y, 0, p2x, p2y, 0, color);
		p1x = p2x;
		p1y = p2y;
	}

	p2x = a_x + b_x + c_x + d_x;
	p2y = a_y + b_y + c_y + d_y;
	add_edge(pm, p1x, p1y, 0, p2x, p2y, 0, color);
}

void add_box(
	point_matrix pm,
	double x, double y, double z,
	double w, double h, double d,
	uint32_t color
	) {
	double x2 = x + w, y2 = y + h, z2 = z + d;
	add_edge(pm, x, y, z, x2, y, z, color);
	add_edge(pm, x, y, z, x, y2, z, color);
	add_edge(pm, x, y, z, x, y, z2, color);
	add_edge(pm, x2, y, z, x2, y2, z, color);
	add_edge(pm, x2, y, z, x2, y, z2, color);
	add_edge(pm, x, y2, z, x2, y2, z, color);
	add_edge(pm, x, y2, z, x, y2, z2, color);
	add_edge(pm, x, y, z2, x2, y, z2, color);
	add_edge(pm, x, y, z2, x, y2, z2, color);
	add_edge(pm, x2, y2, z, x2, y2, z2, color);
	add_edge(pm, x2, y, z2, x2, y2, z2, color);
	add_edge(pm, x, y2, z2, x2, y2, z2, color);
}

void add_sphere(
	point_matrix pm,
	double cx, double cy, double cz,
	double r, int steps, uint32_t color
	) {
	int half_steps = steps / 2;
	double phi = 0.0, phi_inc = M_PI / half_steps;
	double theta, theta_inc = 2 * M_PI / steps;
	double pre_xy1, pre_xy2;
	double x1 = cx, y1 = cy, z1 = cz + r, x2, y2, z2, x3, y3, z3;
	int step_count, half_step_count;

	phi += phi_inc;
	pre_xy2 = r * sin(phi);
	x2 = cx + pre_xy2;
	y2 = cy;
	z2 = z3 = cz + r * cos(phi);
	theta = 0;
	for (step_count = 1; step_count < steps; step_count++) {
		theta += theta_inc;
		x3 = cx + pre_xy2 * cos(theta);
		y3 = cy + pre_xy2 * sin(theta);
		add_edge(pm, x1, y1, z1, x3, y3, z3, color);
		add_edge(pm, x2, y2, z2, x3, y3, z3, color);
		x2 = x3;
		y2 = y3;
	}
	x3 = cx + pre_xy2;
	y3 = cy;
	add_edge(pm, x1, y1, z1, x3, y3, z3, color);
	add_edge(pm, x2, y2, z2, x3, y3, z3, color);

	for (half_step_count = 2; half_step_count < half_steps; half_step_count++) {
		pre_xy1 = pre_xy2;
		z1 = z3;
		phi += phi_inc;
		pre_xy2 = r * sin(phi);
		x2 = cx + pre_xy2;
		y2 = cy;
		z2 = z3 = cz + r * cos(phi);
		theta = 0;
		for (step_count = 1; step_count < steps; step_count++) {
			theta += theta_inc;
			x1 = cx + pre_xy1 * cos(theta);
			y1 = cy + pre_xy1 * sin(theta);
			x3 = cx + pre_xy2 * cos(theta);
			y3 = cy + pre_xy2 * sin(theta);
			add_edge(pm, x1, y1, z1, x3, y3, z3, color);
			add_edge(pm, x2, y2, z2, x3, y3, z3, color);
			x2 = x3;
			y2 = y3;
		}
		x1 = cx + pre_xy1;
		y1 = cy;
		x3 = cx + pre_xy2;
		y3 = cy;
		add_edge(pm, x1, y1, z1, x3, y3, z3, color);
		add_edge(pm, x2, y2, z2, x3, y3, z3, color);
	}

	pre_xy1 = pre_xy2;
	z1 = z3;
	x3 = cx;
	y3 = cy;
	z3 = cz - r;
	theta = 0;
	for (step_count = 1; step_count < steps; step_count++) {
		theta += theta_inc;
		x1 = cx + pre_xy1 * cos(theta);
		y1 = cy + pre_xy1 * sin(theta);
		add_edge(pm, x1, y1, z1, x3, y3, z3, color);
	}
	x1 = cx + pre_xy1;
	y1 = cy;
	add_edge(pm, x1, y1, z1, x3, y3, z3, color);
}

void add_torus(
	point_matrix pm,
	double cx, double cy, double cz,
	double r1, double r2,
	int steps, uint32_t color
	) {
	double alpha = 0, alpha_inc = 2 * M_PI / steps;
	double beta, beta_inc = 2 * M_PI / steps;
	int step_count1, step_count2;
	for (step_count2 = 0; step_count2 < steps; step_count2++) {
		beta = 0;
		for(step_count1 = 0; step_count1 < steps; step_count1++){
			add_edge(
				pm,
				cx + (r2 + r1 * sin(beta)) * cos(alpha),
				cy + r1 * cos(beta),
				cz + (r2 + r1 * sin(beta)) * sin(alpha),
				cx + (r2 + r1 * sin(beta)) * cos(alpha),
				cy + r1 * cos(beta),
				cz + (r2 + r1 * sin(beta)) * sin(alpha),
				color
				);
			beta += beta_inc;
		}
		alpha += alpha_inc;
	}
}
