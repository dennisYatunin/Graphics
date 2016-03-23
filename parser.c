#include "parser.h"
#include "draw.h"
#include "png.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_msg(const char *msg) {
	fprintf(stderr, "Parse error: %s\n", msg);
	exit(EXIT_FAILURE);
}

void parse(const char *name) {
	FILE *fp;
	char line[256];

	if (strcmp(name, "stdin") == 0) {
		fp = stdin;
	}
	else {
		fp = fopen(name, "r");
		if (fp == NULL) {
			perror("Parse error (fopen)");
			exit(EXIT_FAILURE);
		}
	}

	// Default values
	char color_type = PNG_RGB;
	int width = 1000, height = 1000, color = rgb(255, 255, 255), segs = 50;
	point_matrix pm = make_point_matrix(100);
	matrix transform = identity_matrix(4);

	double d1, d2, d3, d4, d5, d6, d7, d8;
	int i1, i2, i3, i4;
	matrix temp;
	screen s;
	while (fgets(line, 255, fp) != NULL) {
		if (strcmp(line, "quit\n") == 0) {
			return;
		}
		if (strcmp(line, "setwh\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%d %d\n", &i1, &i2) < 2
				) {
				error_msg("expected width and height after \"setwh\"");
			}
			width = i1;
			height = i2;
		}
		else if (strcmp(line, "setct\n") == 0) {
			if (fgets(line, 255, fp) == NULL) {
				error_msg("expected \"PNG\" or \"PNGA\" after \"setct\"");
			}
			if (strcmp(line, "RGB\n") == 0) {
				color_type = PNG_RGB;
			}
			else if (strcmp(line, "RGBA\n") == 0) {
				color_type = PNG_RGBA;
			}
			else {
				error_msg("expected \"PNG\" or \"PNGA\" after \"setct\"");
			}
		}
		else if (strcmp(line, "setcolor\n") == 0) {
			if (color_type == PNG_RGB) {
				if (
					fgets(line, 255, fp) == NULL ||
					sscanf(line,"%d %d %d\n", &i1, &i2, &i3) < 3
					) {
					error_msg("expected r, g, b after \"setcolor\"");
				}
				color = rgb(i1, i2, i3);
			}
			else {
				if (
					fgets(line, 255, fp) == NULL ||
					sscanf(line,"%d %d %d %d\n", &i1, &i2, &i3, &i4) < 4
					) {
					error_msg("expected r, g, b, a after \"setcolor\"");
				}
				color = rgba(i1, i2, i3, i4);
			}
		}
		else if (strcmp(line, "setsegs\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%d\n", &i1) < 1
				) {
				error_msg("expected number of segments after \"setsegs\"");
			}
			segs = i1;
		}
		else if (strcmp(line, "display\n") == 0) {
			s = make_screen(width, height);
			draw_lines(pm, s);
			display_png(s, color_type);
			free_screen(s);
		}
		else if (strcmp(line, "save\n") == 0) {
			if (fgets(line, 255, fp) == NULL) {
				error_msg("expected file name after \"save\"");
			}
			s = make_screen(width, height);
			draw_lines(pm, s);
			make_png(line, s, color_type);
			free_screen(s);
		}
		else if (strcmp(line, "line\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(
					line,"%lf %lf %lf %lf %lf %lf\n",
					&d1, &d2, &d3, &d4, &d5, &d6
					) < 6
				) {
				error_msg("expected x0, y0, z0, x1, y1, z1 after \"line\"");
			}
			add_edge(pm, d1, d2, d3, d4, d5, d6, color);
		}
		else if (strcmp(line, "circle\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected cx, cy, r after \"circle\"");
			}
			add_circle(pm, d1, d2, d3, segs, color);
		}
		else if (strcmp(line, "hermite\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(
					line,"%lf %lf %lf %lf %lf %lf %lf %lf\n",
					&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8
					) < 8
				) {
				error_msg(
					"expected x0, y0, x1, y1, x2, y2, x3, y3 after "
					"\"hermite\""
					);
			}
			add_curve(pm, d1, d2, d3, d4, d5, d6, d7, d8, segs, HERMIT, color);
		}
		else if (strcmp(line, "bezier\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(
					line,"%lf %lf %lf %lf %lf %lf %lf %lf\n",
					&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8
					) < 8
				) {
				error_msg(
					"expected x0, y0, x1, y1, x2, y2, x3, y3 after "
					"\"bezier\""
					);
			}
			add_curve(pm, d1, d2, d3, d4, d5, d6, d7, d8, segs, BEZIER, color);
		}
		else if (strcmp(line, "ident\n") == 0) {
			transform = identity_matrix(4);
		}
		else if (strcmp(line, "scale\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected sx, sy, sz after \"scale\"");
			}
			temp = scale_matrix(d1, d2, d3);
			matrix_multiply(temp, transform);
			free_matrix(temp);
		}
		else if (strcmp(line, "translate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected tx, ty, tz after \"translate\"");
			}
			temp = translation_matrix(d1, d2, d3);
			matrix_multiply(temp, transform);
			free_matrix(temp);
		}
		else if (strcmp(line, "xrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"xrotate\"");
			}
			temp = rotation_matrix_x(d1);
			matrix_multiply(temp, transform);
			free_matrix(temp);
		}
		else if (strcmp(line, "yrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"yrotate\"");
			}
			temp = rotation_matrix_y(d1);
			matrix_multiply(temp, transform);
			free_matrix(temp);
		}
		else if (strcmp(line, "zrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"zrotate\"");
			}
			temp = rotation_matrix_z(d1);
			matrix_multiply(temp, transform);
			free_matrix(temp);
		}
		else if (strcmp(line, "apply\n") == 0) {
			matrix_multiply(transform, pm->points);
		}
		else {
			line[strlen(line) - 1] = '\0';
			fprintf(stderr, "Parse error: invalid command (\"%s\")\n", line);
			exit(EXIT_FAILURE);
		}
	}
}
