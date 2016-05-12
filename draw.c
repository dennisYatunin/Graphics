#include "draw.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

vertex_list new_vlist(size_t max_num_vertices) {
	vertex_list vlist = (vertex_list) malloc(sizeof(struct vertex_list_struct));
	if (vlist == NULL) {
		perror("New VList error (malloc)");
		exit(EXIT_FAILURE);
	}
	vlist->size = 0;
	vlist->capacity = max_num_vertices;
	vlist->list = (vertex *) malloc(max_num_vertices * sizeof(vertex));
	if (vlist->list == NULL) {
		perror("New VList error (malloc)");
		exit(EXIT_FAILURE);
	}
	return vlist;
}

void free_vlist(vertex_list vlist) {
	free(vlist->list);
	free(vlist);
}

void clear_vlist(vertex_list vlist) {
	vlist->size = 0;
}

void add_vertex(
	vertex_list vlist, double x, double y, double z, uint32_t color
	) {
	if (vlist->size == vlist->capacity) {
		vlist->capacity *= 2;
		vlist->list = realloc(vlist->list, vlist->capacity * sizeof(vertex));
		if (vlist->list == NULL) {
			perror("Add Vertex error (realloc)");
			exit(EXIT_FAILURE);
		}
	}
	vertex v;
	v.x = x; v.y = y; v.z = z; v.color = color;
	vlist->list[vlist->size++] = v;
}

vertex *transformed_vertices(vertex_list vlist, matrix m) {
	vertex *old_list = vlist->list;
	vertex *new_list = (vertex *) malloc(vlist->capacity * sizeof(vertex));
	if (new_list == NULL) {
		perror("Transform error (malloc)");
		exit(EXIT_FAILURE);
	}
	vertex *new_list_copy = new_list;
	matrix m_copy = m;
	vertex old_v, new_v;
	size_t counter = vlist->size;
	while (counter--) {
		old_v = *old_list++;
		m = m_copy;
		new_v.x = old_v.x * *m++ + old_v.y * *m++ + old_v.z * *m++ + *m++;
		new_v.y = old_v.x * *m++ + old_v.y * *m++ + old_v.z * *m++ + *m++;
		new_v.z = old_v.x * *m++ + old_v.y * *m++ + old_v.z * *m++ + *m;
		new_v.color = old_v.color;
		*new_list++ = new_v;
	}
	return new_list_copy;
}

void transform(vertex_list vlist, matrix m) {
	vertex *new_list = transformed_vertices(vlist, m);
	free(vlist->list);
	vlist->list = new_list;
}

face_list new_flist(size_t max_num_faces) {
	face_list flist = (face_list) malloc(sizeof(struct face_list_struct));
	if (flist == NULL) {
		perror("New FList error (malloc)");
		exit(EXIT_FAILURE);
	}
	flist->size = 0;
	flist->capacity = max_num_faces;
	flist->list = (face *) malloc(max_num_faces * sizeof(face));
	if (flist->list == NULL) {
		perror("New FList error (malloc)");
		exit(EXIT_FAILURE);
	}
	return flist;
}

void free_flist(face_list flist) {
	free(flist->list);
	free(flist);
}

void clear_flist(face_list flist) {
	flist->size = 0;
}

void add_face(face_list flist, size_t v0, size_t v1, size_t v2) {
	if (flist->size == flist->capacity) {
		flist->capacity *= 2;
		flist->list = realloc(flist->list, flist->capacity * sizeof(face));
		if (flist->list == NULL) {
			perror("Add Face error (realloc)");
			exit(EXIT_FAILURE);
		}
	}
	face f;
	f.v0 = v0; f.v1 = v1; f.v2 = v2;
	flist->list[flist->size++] = f;
}

edge_list new_elist(size_t max_num_edges) {
	edge_list elist = (edge_list) malloc(sizeof(struct edge_list_struct));
	if (elist == NULL) {
		perror("New EList error (malloc)");
		exit(EXIT_FAILURE);
	}
	elist->size = 0;
	elist->capacity = max_num_edges;
	elist->list = (edge *) malloc(max_num_edges * sizeof(edge));
	if (elist->list == NULL) {
		perror("New EList error (malloc)");
		exit(EXIT_FAILURE);
	}
	return elist;
}

void free_elist(edge_list elist) {
	free(elist->list);
	free(elist);
}

void clear_elist(edge_list elist) {
	elist->size = 0;
}

void add_edge(edge_list elist, size_t v0, size_t v1) {
	if (elist->size == elist->capacity) {
		elist->capacity *= 2;
		elist->list = realloc(elist->list, elist->capacity * sizeof(edge));
		if (elist->list == NULL) {
			perror("Add Edge error (realloc)");
			exit(EXIT_FAILURE);
		}
	}
	edge e;
	e.v0 = v0; e.v1 = v1;
	elist->list[elist->size++] = e;
}

void draw_line(
	screen s,
	int32_t x0, int32_t y0, uint32_t color0,
	int32_t x1, int32_t y1, uint32_t color1
	) {
	int32_t dx = abs(x1 - x0);
	int32_t dy = abs(y1 - y0);

	int32_t x_step = x0 < x1 ? 1 : -1;
	int32_t y_step = y0 < y1 ? 1 : -1;

	int32_t error = (dx > dy ? dx : -dy) / 2;
	int32_t original_error;
	if (color0 == color1) {
		while (1) {
			plot(s, x0, y0, color0);
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
	// this could probably be done more efficiently, but I'm too lazy
	else {
		int32_t error_copy = error, x0_copy = x0, y0_copy = y0;
		size_t num_steps = 0;
		int8_t r0 = (int8_t) (color0 >> 24 & 0xFF);
		int8_t dr = (int8_t) (color1 >> 24 & 0xFF) - r0;
		int8_t g0 = (int8_t) (color0 >> 16 & 0xFF);
		int8_t dg = (int8_t) (color1 >> 16 & 0xFF) - g0;
		int8_t b0 = (int8_t) (color0 >> 8  & 0xFF);
		int8_t db = (int8_t) (color1 >> 8  & 0xFF) - b0;
		int8_t a0 = (int8_t) (color0       & 0xFF);
		int8_t da = (int8_t) (color1       & 0xFF) - a0;
		while (1) {
			num_steps++;
			if (x0_copy == x1 && y0_copy == y1) {
				num_steps--;
				break;
			}
			original_error = error;
			if (original_error > -dx) {
				error_copy -= dy;
				x0_copy += x_step;
			}
			if (original_error < dy) {
				error_copy += dx;
				y0_copy += y_step;
			}
		}
		size_t num_steps_copy = 0;
		while (1) {
			plot(s, x0, y0,
				(uint8_t) (r0 + dr * num_steps_copy / num_steps) << 24 |
				(uint8_t) (g0 + dg * num_steps_copy / num_steps) << 16 |
				(uint8_t) (b0 + db * num_steps_copy / num_steps) << 8  |
				(uint8_t) (a0 + da * num_steps_copy / num_steps)
				);
			num_steps_copy++;
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
}

void draw_line_perspective(screen s, vertex v0, vertex v1) {
	if (v0.z < 0 && v1.z < 0) {
		draw_line(s, v0.x, v0.y, v0.color, v1.x, v1.y, v1.color);
		return;
	}

	if (v0.z >= 0 && v1.z >= 0) {
		return;
	}

	if (v0.z >= 0) {
		double factor = -v1.z / (v0.z - v1.z) * 0.99999999;
		// multiply by 0.99999999 to avoid making the new v0.z being 0
		draw_line(
			s,
			factor * v0.x + (1 - factor) * v1.x,
			factor * v0.y + (1 - factor) * v1.y, v0.color,
			v1.x, v1.y, v1.color
			);
	}

	else {
		double factor = -v0.z / (v1.z - v0.z) * 0.99999999;
		// multiply by 0.99999999 to avoid making the new v1.z being 0
		draw_line(
			s,
			v0.x, v0.y, v0.color,
			(1 - factor) * v0.x + factor * v1.x,
			(1 - factor) * v0.y + factor * v1.y, v1.color
			);
	}
}

char is_front_face(vertex v0, vertex v1, vertex v2) {
	// Vector a points from v0 to v1, and vector b points from v0 to v2.
	double a_x = v1.x - v0.x, a_y = v1.y - v0.y,
	b_x = v2.x - v0.x, b_y = v2.y - v0.y;
	// Since the eye is always looking down the z-axis (i.e. all visible points
	// have negative z-coords) and the vertices are given in counter-clockwise
	// order, a front face is a face such that the cross product of a and b
	// points at the eye (i.e. has a positive z-component).
	return a_x * b_y - a_y * b_x > 0;
}

void draw_faces(
	screen s, vertex_list vlist, face_list flist,
	vector center, vector eye, double distance
	) {
	size_t num_faces = flist->size;
	face *faces = flist->list;
	matrix display = display_matrix(center, eye);
	vertex *vertices = transformed_vertices(vlist, display);
	free(display);
	face f;
	vertex v0, v1, v2;
	if (distance) {
		int num_vertices = vlist->size, cur_vertex = 0;
		double depths[num_vertices], scale_factor;
		vertex *vertices_cpy = vertices;
		while (cur_vertex < num_vertices) {
			if (vertices_cpy->z < 0) {
				scale_factor = -distance / vertices_cpy->z;
			}
			else {
				scale_factor = distance / vertices_cpy->z;
			}
			vertices_cpy->x *= scale_factor;
			vertices_cpy->y *= scale_factor;
			vertices_cpy++;
			cur_vertex++;
		}
		while (num_faces--) {
			f = *faces++;
			v0 = vertices[f.v0]; v1 = vertices[f.v1]; v2 = vertices[f.v2];
			if (is_front_face(v0, v1, v2)) {
				draw_line_perspective(s, v0, v1);
				draw_line_perspective(s, v1, v2);
				draw_line_perspective(s, v2, v0);
			}
		}
	}
	else {
		while (num_faces--) {
			f = *faces++;
			v0 = vertices[f.v0]; v1 = vertices[f.v1]; v2 = vertices[f.v2];
			if (is_front_face(v0, v1, v2)) {
				draw_line(s, v0.x, v0.y, v0.color, v1.x, v1.y, v1.color);
				draw_line(s, v1.x, v1.y, v1.color, v2.x, v2.y, v2.color);
				draw_line(s, v2.x, v2.y, v2.color, v0.x, v0.y, v0.color);
			}
		}
	}
	free(vertices);
}

void add_box(
	vertex_list vlist, face_list flist,
	double x, double y, double z,
	double w, double h, double d,
	uint32_t color
	) {
	size_t first_index = vlist->size;

	double x2 = x + w, y2 = y - h, z2 = z - d;
	add_vertex(vlist, x , y , z , color);
	add_vertex(vlist, x , y2, z , color);
	add_vertex(vlist, x , y2, z2, color);
	add_vertex(vlist, x , y , z2, color);
	add_vertex(vlist, x2, y , z , color);
	add_vertex(vlist, x2, y2, z , color);
	add_vertex(vlist, x2, y2, z2, color);
	add_vertex(vlist, x2, y , z2, color);

	// planes perpendicular to x = constant
	add_face(flist, first_index    , first_index + 5, first_index + 4);
	add_face(flist, first_index    , first_index + 1, first_index + 5);
	add_face(flist, first_index + 3, first_index + 1, first_index    );
	add_face(flist, first_index + 3, first_index + 2, first_index + 1);
	add_face(flist, first_index + 7, first_index + 2, first_index + 3);
	add_face(flist, first_index + 7, first_index + 6, first_index + 2);
	add_face(flist, first_index + 4, first_index + 6, first_index + 7);
	add_face(flist, first_index + 4, first_index + 5, first_index + 6);

	// planes parallel to x = constant
	add_face(flist, first_index    , first_index + 7, first_index + 3);
	add_face(flist, first_index    , first_index + 4, first_index + 7);
	add_face(flist, first_index + 2, first_index + 5, first_index + 1);
	add_face(flist, first_index + 2, first_index + 6, first_index + 5);
}

void add_sphere(
	vertex_list vlist, face_list flist,
	double cx, double cy, double cz,
	double r, int steps, uint32_t color
	) {
	size_t first_index = vlist->size;
	add_vertex(vlist, cx, cy, cz + r, color);

	int step_count, half_step_count;
	int half_steps = steps / 2;
	double phi = 0.0, phi_inc = M_PI / half_steps;
	double theta, theta_inc = 2 * M_PI / steps;
	double pre_xy, z;
	for (half_step_count = 1; half_step_count < half_steps; half_step_count++) {
		phi += phi_inc;
		pre_xy = r * sin(phi);
		z = cz + r * cos(phi);
		add_vertex(vlist, cx + pre_xy, cy, z, color);
		theta = 0;
		for (step_count = 1; step_count < steps; step_count++) {
			theta += theta_inc;
			add_vertex(
				vlist,
				cx + pre_xy * cos(theta), cy + pre_xy * sin(theta), z, color
				);
		}
	}

	add_vertex(vlist, cx, cy, cz - r, color);

	for (step_count = 1; step_count < steps; step_count++) {
		add_face(
			flist,
			first_index, first_index + step_count, first_index + step_count + 1
			);
	}
	add_face(flist, first_index, first_index + step_count, first_index + 1);

	half_steps -= 2;
	for (half_step_count = 0; half_step_count < half_steps; half_step_count++) {
		for (step_count = 1; step_count < steps; step_count++) {
			add_face(
				flist,
				first_index + half_step_count * steps + step_count,
				first_index + half_step_count * steps + steps + step_count,
				first_index + half_step_count * steps + steps + step_count + 1
				);
			add_face(
				flist,
				first_index + half_step_count * steps + step_count,
				first_index + half_step_count * steps + steps + step_count + 1,
				first_index + half_step_count * steps + step_count + 1
				);
		}
		add_face(
			flist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + step_count,
			first_index + half_step_count * steps + steps + 1
			);
		add_face(
			flist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + 1,
			first_index + half_step_count * steps + 1
			);
	}

	for (step_count = 1; step_count < steps; step_count++) {
		add_face(
			flist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + 1,
			first_index + half_step_count * steps + step_count + 1
			);
	}
	add_face(
		flist,
		first_index + half_step_count * steps + step_count,
		first_index + half_step_count * steps + steps + 1,
		first_index + half_step_count * steps + 1
		);
}

void draw_edges(
	screen s, vertex_list vlist, edge_list elist,
	vector center, vector eye, double distance
	) {
	size_t num_edges = elist->size;
	edge *edges = elist->list;
	matrix display = display_matrix(center, eye);
	vertex *vertices = transformed_vertices(vlist, display);
	free(display);
	edge e;
	vertex v0, v1;
	if (distance) {
		distance *= -1;
		while (num_edges--) {
			e = *edges++;
			v0 = vertices[e.v0]; v1 = vertices[e.v1];
			v0.x *= distance / v0.z; v0.y *= distance / v0.z;
			v1.x *= distance / v1.z; v1.y *= distance / v1.z;
			draw_line_perspective(s, v0, v1);
		}
	}
	else {
		while (num_edges--) {
			e = *edges++;
			v0 = vertices[e.v0]; v1 = vertices[e.v1];
			draw_line(s, v0.x, v0.y, v0.color, v1.x, v1.y, v1.color);
		}
	}
	free(vertices);
}

void add_line(
	vertex_list vlist, edge_list elist,
	double x0, double y0, double z0, double x1, double y1, double z1,
	uint32_t color
	) {
	size_t first_index = elist->size;

	add_vertex(vlist, x0, y0, z0, color);
	add_vertex(vlist, x1, y1, z1, color);

	add_edge(elist, first_index, first_index + 1);
}

/*
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
*/
