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
	char color_type = PNG_RGB, perspective = PERSPECTIVE_OFF;
	int width = 1000, height = 1000, color = rgb(255, 255, 255), segs = 20;
	screen s = make_screen(width, height);
	vertex_list vlist = new_vlist(100);
	face_list flist = new_flist(100);
	stack st = new_stack(5);
	vector center; center.x = 0; center.y = 0; center.z = 0;
	vector view; view.x = 0; view.y = 0; view.z = -2;
	double distance = 1;
	matrix perp = perp_matrix(center, view, distance);

	double d1, d2, d3, d4, d5, d6, d7, d8;
	int i1, i2, i3, i4;
	matrix temp;
	while (fgets(line, 255, fp) != NULL) {
		if (*line == '#') {
			continue;
		}
		if (strcmp(line, "quit\n") == 0) {
			return;
		}
		else if (strcmp(line, "clear\n") == 0) {
			clear_vlist(vlist);
			clear_flist(flist);
			clear_stack(st);
		}
		if (strcmp(line, "setwh\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%d %d\n", &i1, &i2) < 2
				) {
				error_msg("expected width and height after \"setwh\"");
			}
			width = i1; height = i2;
			free_screen(s);
			s = make_screen(width, height);
		}
		else if (strcmp(line, "setperspective\n") == 0) {
			if (fgets(line, 255, fp) == NULL) {
				error_msg(
					"expected \"on\" or \"off\" after \"setperspective\""
					);
			}
			if (strcmp(line, "on\n") == 0) {
				perspective = PERSPECTIVE_ON;
			}
			else if (strcmp(line, "off\n") == 0) {
				perspective = PERSPECTIVE_OFF;
			}
			else {
				error_msg(
					"expected \"on\" or \"off\" after \"setperspective\""
					);
			}
		}
		else if (strcmp(line, "setcenter\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected x, y, z after \"setcenter\"");
			}
			center.x = d1; center.y = d2; center.z = d3;
			perp = perp_matrix(center, view, distance);
		}
		else if (strcmp(line, "setcamera\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected x, y, z after \"setcamera\"");
			}
			view.x = center.x - d1;
			view.y = center.y - d2;
			view.z = center.z - d3;
			perp = perp_matrix(center, view, distance);
		}
		else if (strcmp(line, "setdistance\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected distance after \"setdistance\"");
			}
			distance = d1;
			perp = perp_matrix(center, view, distance);
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
			transform(vlist, peek(st));
			draw_polygons(s, vlist, flist, perp, perspective);
			clear_vlist(vlist);
			clear_flist(flist);
			display_png(s, color_type);
		}
		else if (strcmp(line, "save\n") == 0) {
			if (fgets(line, 255, fp) == NULL) {
				error_msg("expected file name after \"save\"");
			}
			transform(vlist, peek(st));
			draw_polygons(s, vlist, flist, perp, perspective);
			clear_vlist(vlist);
			clear_flist(flist);
			make_png(line, s, color_type);
		}
		/*
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
					"expected x0, y0, x1, y1, x2, y2, x3, y3 after \"hermite\""
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
					"expected x0, y0, x1, y1, x2, y2, x3, y3 after \"bezier\""
					);
			}
			add_curve(pm, d1, d2, d3, d4, d5, d6, d7, d8, segs, BEZIER, color);
		}
		*/
		else if (strcmp(line, "box\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(
					line,"%lf %lf %lf %lf %lf %lf\n",
					&d1, &d2, &d3, &d4, &d5, &d6
					) < 6
				) {
				error_msg("expected x, y, z, h, w, d after \"box\"");
			}
			add_box(vlist, flist, d1, d2, d3, d4, d5, d6, color);
		}
		else if (strcmp(line, "sphere\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf %lf\n", &d1, &d2, &d3, &d4) < 4
				) {
				error_msg("expected cx, cy, cz, r after \"sphere\"");
			}
			add_sphere(vlist, flist, d1, d2, d3, d4, segs, color);
		}
		/*
		else if (strcmp(line, "torus\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(
					line,"%lf %lf %lf %lf %lf\n",
					&d1, &d2, &d3, &d4, &d5
					) < 5
				) {
				error_msg("expected cx, cy, cz, r1, r2 after \"torus\"");
			}
			add_torus(pm, d1, d2, d3, d4, d5, segs, color);
		}
		*/
		else if (strcmp(line, "push\n") == 0) {
			transform(vlist, peek(st));
			draw_polygons(s, vlist, flist, perp, perspective);
			clear_vlist(vlist);
			clear_flist(flist);
			push(st);
		}
		else if (strcmp(line, "pop\n") == 0) {
			transform(vlist, peek(st));
			draw_polygons(s, vlist, flist, perp, perspective);
			clear_vlist(vlist);
			clear_flist(flist);
			pop(st);
		}
		else if (strcmp(line, "scale\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected sx, sy, sz after \"scale\"");
			}
			scale(d1, d2, d3, peek(st));
		}
		else if (strcmp(line, "translate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf %lf %lf\n", &d1, &d2, &d3) < 3
				) {
				error_msg("expected i, j, k after \"translate\"");
			}
			translate(d1, d2, d3, peek(st));
		}
		else if (strcmp(line, "xrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"xrotate\"");
			}
			rotate_x(d1, peek(st));
		}
		else if (strcmp(line, "yrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"yrotate\"");
			}
			rotate_y(d1, peek(st));
		}
		else if (strcmp(line, "zrotate\n") == 0) {
			if (
				fgets(line, 255, fp) == NULL ||
				sscanf(line,"%lf\n", &d1) < 1
				) {
				error_msg("expected theta after \"zrotate\"");
			}
			rotate_y(d1, peek(st));
		}
		else {
			line[strlen(line) - 1] = '\0';
			fprintf(stderr, "Parse error: invalid command (\"%s\")\n", line);
			exit(EXIT_FAILURE);
		}
	}
}
