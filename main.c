#include "draw.h"
#include "png.h"

#include <stdio.h>
int main() {
	screen s = make_screen(121, 1001);
	point_matrix pm = make_point_matrix(28);
	// H
	add_edge(pm, 0, 0, 0, 0, 50, 0);
	add_edge(pm, 25, 0, 0, 25, 50, 0);
	add_edge(pm, 0, 25, 0, 25, 25, 0);
	// e
	add_edge(pm, 30, 10, 0, 55, 10, 0);
	add_edge(pm, 55, 10, 0, 55, 25, 0);
	add_edge(pm, 55, 25, 0, 30, 25, 0);
	add_edge(pm, 30, 25, 0, 30, 0, 0);
	add_edge(pm, 30, 0, 0, 55, 0, 0);
	// l
	add_edge(pm, 72, 0, 0, 72, 50, 0);
	// l
	add_edge(pm, 102, 0, 0, 102, 50, 0);
	// o
	add_edge(pm, 120, 0, 0, 145, 0, 0);
	add_edge(pm, 145, 0, 0, 145, 25, 0);
	add_edge(pm, 145, 25, 0, 120, 25, 0);
	add_edge(pm, 120, 25, 0, 120, 0, 0);
	//matrix_print(pm->points);

	int counter;
	matrix T;
	for (counter = 0; counter <= 20; counter++) {
		T = translation_matrix(-60, 50, 0);
		matrix_multiply(rotation_matrix_y(18), T);
		matrix_multiply(translation_matrix(60, 0, 0), T);
		matrix_multiply(T, pm->points);
		draw_lines(pm, s);
	}

	make_png("adelle.png", s->width, s->height, s->data, PNG_RGB);
}