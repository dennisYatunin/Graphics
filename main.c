#include "draw.h"
#include "png.h"

#include <stdio.h>
int main() {
	screen s = make_screen(154, 551);
	point_matrix pm = make_point_matrix(28);
	// H
	add_edge(pm, 4, 0, 0, 4, 50, 0);
	add_edge(pm, 29, 0, 0, 29, 50, 0);
	add_edge(pm, 4, 25, 0, 29, 25, 0);
	// e
	add_edge(pm, 34, 10, 0, 59, 10, 0);
	add_edge(pm, 59, 10, 0, 59, 25, 0);
	add_edge(pm, 59, 25, 0, 34, 25, 0);
	add_edge(pm, 34, 25, 0, 34, 0, 0);
	add_edge(pm, 34, 0, 0, 59, 0, 0);
	// l
	add_edge(pm, 76, 0, 0, 76, 50, 0);
	// l
	add_edge(pm, 106, 0, 0, 106, 50, 0);
	// o
	add_edge(pm, 124, 0, 0, 149, 0, 0);
	add_edge(pm, 149, 0, 0, 149, 25, 0);
	add_edge(pm, 149, 25, 0, 124, 25, 0);
	add_edge(pm, 124, 25, 0, 124, 0, 0);
	draw_lines(pm, s);

	int counter;
	matrix T = translation_matrix(-77, 50, 0);
	matrix_multiply(rotation_matrix_y(18), T);
	matrix_multiply(translation_matrix(77, 0, 0), T);
	for (counter = 0; counter < 10; counter++) {
		matrix_multiply(T, pm->points);
		draw_lines(pm, s);
	}

	make_png("adelle.png", s, PNG_RGB);
}