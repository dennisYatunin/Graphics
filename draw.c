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
	vertex_list vlist, double x, double y, double z,
	double nx, double ny, double nz, vertex text0, vertex text1
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
	v.x = x; v.y = y; v.z = z; v.nx = nx; v.ny = ny; v.nz = nz;
	double coef = (
		(x - text0.x) * text0.nx +
		(y - text0.y) * text0.ny +
		(z - text0.z) * text0.nz
		) / text1.nx;
	if (coef < 0) {
		coef = 0;
	}
	if (coef > 1) {
		coef = 1;
	}
	v.shine = (1 - coef) * text0.shine + coef * text1.shine;
	v.color = rgb(
		(uint8_t) (
			(1 - coef) * (text0.color >> 24) +
			coef * (text1.color >> 24)
			),
		(uint8_t) (
			(1 - coef) * (text0.color << 8 >> 24) +
			coef * (text1.color << 8 >> 24)
			),
		(uint8_t) (
			(1 - coef) * (text0.color << 16 >> 24) +
			coef * (text1.color << 16 >> 24)
			)
		);
	vlist->list[vlist->size++] = v;
}

vector transformed_vector(vector v, matrix m) {
	vector new_v;
	new_v.x = v.x * *m++ + v.y * *m++ + v.z * *m++ + *m++;
	new_v.y = v.x * *m++ + v.y * *m++ + v.z * *m++ + *m++;
	new_v.z = v.x * *m++ + v.y * *m++ + v.z * *m++ + *m;
	return new_v;
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

		new_v.x = old_v.x * *m; new_v.nx = old_v.nx * *m++;
		new_v.x += old_v.y * *m; new_v.nx += old_v.ny * *m++;
		new_v.x += old_v.z * *m; new_v.nx += old_v.nz * *m++;
		new_v.x += *m++;

		new_v.y = old_v.x * *m; new_v.ny = old_v.nx * *m++;
		new_v.y += old_v.y * *m; new_v.ny += old_v.ny * *m++;
		new_v.y += old_v.z * *m; new_v.ny += old_v.nz * *m++;
		new_v.y += *m++;

		new_v.z = old_v.x * *m; new_v.nz = old_v.nx * *m++;
		new_v.z += old_v.y * *m; new_v.nz += old_v.ny * *m++;
		new_v.z += old_v.z * *m; new_v.nz += old_v.nz * *m++;
		new_v.z += *m;

		new_v.shine = old_v.shine;
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

void draw_pixel(
	screen s, int32_t x, int32_t y, double z,
	uint32_t point_color, double point_shine,
	uint32_t amb_color, uint32_t light_color, vector point_to_source,
	vector normal, vector point_to_eye, vector reflected,
	double amb_ref_constant, double diff_ref_constant, double spec_ref_constant
	) {
	double r = amb_ref_constant *
	(point_color >> 24) * (amb_color >> 24) / 0xFF;
	double g = amb_ref_constant *
	(point_color << 8 >> 24) * (amb_color << 8 >> 24) / 0xFF;
	double b = amb_ref_constant *
	(point_color << 16 >> 24) * (amb_color << 16 >> 24) / 0xFF;

	double coef = diff_ref_constant * (
		point_to_source.x * normal.x +
		point_to_source.y * normal.y +
		point_to_source.z * normal.z
		);
	r += coef * (point_color >> 24) * (light_color >> 24) / 0xFF;
	g += coef * (point_color << 8 >> 24) * (light_color << 8 >> 24) / 0xFF;
	b += coef * (point_color << 16 >> 24) * (light_color << 16 >> 24) / 0xFF;

	coef = spec_ref_constant * pow(
		-point_to_eye.x * reflected.x +
		-point_to_eye.y * reflected.y +
		-point_to_eye.z * reflected.z,
		-point_shine
		) / 9000;
	if (coef > 1) {
		coef = 1;
	}
	else if (coef < 0) {
		coef = 1;
	}
	r += coef * (point_color >> 24) * (light_color >> 24) / 0xFF;
	g += coef * (point_color << 8 >> 24) * (light_color << 8 >> 24) / 0xFF;
	b += coef * (point_color << 16 >> 24) * (light_color << 16 >> 24) / 0xFF;
	plot(
		s, x, y, z,
		rgb(
			((uint8_t) min(0xFF, r)),
			((uint8_t) min(0xFF, g)),
			((uint8_t) min(0xFF, b))
			)
		);
}

void scanline_fill_half(
	screen s, int32_t start_y, int32_t end_y,
	vertex l_edge_top_vertex, vertex l_edge_bot_vertex,
	vertex r_edge_top_vertex, vertex r_edge_bot_vertex,
	uint32_t amb_color, uint32_t light_color, vector light_source, vector eye,
	double amb_ref_constant, double diff_ref_constant, double spec_ref_constant,
	char perspective
	) {
	int32_t y, x, start_x, end_x;
	uint32_t start_color, end_color;
	double start_shine, end_shine, coef0, coef1, z, start_z, end_z, mag;
	vector start_normal, end_normal, point_to_source, normal, point_to_eye,
	reflected;

	for (y = start_y; y < end_y; y++) {
		coef0 =
		(y - l_edge_top_vertex.y) / (l_edge_bot_vertex.y - l_edge_top_vertex.y);
		coef1 =
		(y - r_edge_top_vertex.y) / (r_edge_bot_vertex.y - r_edge_top_vertex.y);

		start_z = l_edge_top_vertex.z +
		coef0 * (l_edge_bot_vertex.z - l_edge_top_vertex.z);
		end_z = r_edge_top_vertex.z +
		coef1 * (r_edge_bot_vertex.z - r_edge_top_vertex.z);

		if (perspective == PERSPECTIVE_ON && start_z > 0 && end_z > 0) {
			continue;
		}

		start_x = (int32_t) (
			l_edge_top_vertex.x +
			coef0 * (l_edge_bot_vertex.x - l_edge_top_vertex.x)
			);
		end_x = (int32_t) (
			r_edge_top_vertex.x +
			coef1 * (r_edge_bot_vertex.x - r_edge_top_vertex.x)
			);

		if (perspective == PERSPECTIVE_ON) {
			if (start_z > 0) {
				double factor = -end_z / (start_z - end_z);
				start_x = (int32_t) (
					factor * start_x + (1 - factor) * end_x
					);
			}
			else if (end_z > 0) {
				double factor = -start_z / (end_z - start_z);
				end_x = (int32_t) (
					factor * end_x + (1 - factor) * start_x
					);
			}
		}

		start_color = rgb(
			(uint8_t) (
				(l_edge_top_vertex.color >> 24) + coef0 *
				(
					1. * (l_edge_bot_vertex.color >> 24) -
					(l_edge_top_vertex.color >> 24)
					)
				),
			(uint8_t) (
				(l_edge_top_vertex.color << 8 >> 24) + coef0 *
				(
					1. * (l_edge_bot_vertex.color << 8 >> 24) -
					(l_edge_top_vertex.color << 8 >> 24)
					)
				),
			(uint8_t) (
				(l_edge_top_vertex.color << 16 >> 24) + coef0 *
				(
					1. * (l_edge_bot_vertex.color << 16 >> 24) -
					(l_edge_top_vertex.color << 16 >> 24)
					)
				)
			);

		end_color = rgb(
			(uint8_t) (
				(r_edge_top_vertex.color >> 24) + coef1 *
				(
					1. * (r_edge_bot_vertex.color >> 24) -
					(r_edge_top_vertex.color >> 24)
					)
				),
			(uint8_t) (
				(r_edge_top_vertex.color << 8 >> 24) + coef1 *
				(
					1. * (r_edge_bot_vertex.color << 8 >> 24) -
					(r_edge_top_vertex.color << 8 >> 24)
					)
				),
			(uint8_t) (
				(r_edge_top_vertex.color << 16 >> 24) + coef1 *
				(
					1. * (r_edge_bot_vertex.color << 16 >> 24) -
					(r_edge_top_vertex.color << 16 >> 24)
					)
				)
			);

		start_shine = l_edge_top_vertex.shine +
		coef0 * (l_edge_bot_vertex.shine - l_edge_top_vertex.shine);
		end_shine = r_edge_top_vertex.shine +
		coef1 * (r_edge_bot_vertex.shine - r_edge_top_vertex.shine);

		start_normal.x = l_edge_top_vertex.nx +
		coef0 * (l_edge_bot_vertex.nx - l_edge_top_vertex.nx);
		start_normal.y = l_edge_top_vertex.ny +
		coef0 * (l_edge_bot_vertex.ny - l_edge_top_vertex.ny);
		start_normal.z = l_edge_top_vertex.nz +
		coef0 * (l_edge_bot_vertex.nz - l_edge_top_vertex.nz);
		mag = sqrt(
			start_normal.x * start_normal.x +
			start_normal.y * start_normal.y +
			start_normal.z * start_normal.z
			);
		start_normal.x /= mag;
		start_normal.y /= mag;
		start_normal.z /= mag;

		end_normal.x = r_edge_top_vertex.nx +
		coef1 * (r_edge_bot_vertex.nx - r_edge_top_vertex.nx);
		end_normal.y = r_edge_top_vertex.ny +
		coef1 * (r_edge_bot_vertex.ny - r_edge_top_vertex.ny);
		end_normal.z = r_edge_top_vertex.nz +
		coef1 * (r_edge_bot_vertex.nz - r_edge_top_vertex.nz);
		mag = sqrt(
			end_normal.x * end_normal.x +
			end_normal.y * end_normal.y +
			end_normal.z * end_normal.z
			);
		end_normal.x /= mag;
		end_normal.y /= mag;
		end_normal.z /= mag;

		for (x = start_x; x < end_x; x++) {
			coef0 = 1. * (x - start_x) / (end_x - start_x);
			z = start_z + coef0 * (end_z - start_z);

			point_to_source.x = light_source.x - x;
			point_to_source.y = light_source.y - y;
			point_to_source.z = light_source.z - z;
			mag = sqrt(
				point_to_source.x * point_to_source.x +
				point_to_source.y * point_to_source.y +
				point_to_source.z * point_to_source.z
				);
			point_to_source.x /= mag;
			point_to_source.y /= mag;
			point_to_source.z /= mag;

			normal.x = start_normal.x + coef0 * (end_normal.x - start_normal.x);
			normal.y = start_normal.y + coef0 * (end_normal.y - start_normal.y);
			normal.z = start_normal.z + coef0 * (end_normal.z - start_normal.z);
			mag = sqrt(
				normal.x * normal.x +
				normal.y * normal.y +
				normal.z * normal.z
				);
			normal.x /= mag;
			normal.y /= mag;
			normal.z /= mag;

			point_to_eye.x = -x;
			point_to_eye.y = -y;
			point_to_eye.z = -z;
			mag = sqrt(
				point_to_eye.x * point_to_eye.x +
				point_to_eye.y * point_to_eye.y +
				point_to_eye.z * point_to_eye.z
				);
			point_to_eye.x /= mag;
			point_to_eye.y /= mag;
			point_to_eye.z /= mag;

			coef1 = 2 * (
				point_to_source.x * normal.x +
				point_to_source.y * normal.y +
				point_to_source.z * normal.z
				);
			reflected.x = coef1 * normal.x - point_to_source.x;
			reflected.y = coef1 * normal.y - point_to_source.y;
			reflected.z = coef1 * normal.z - point_to_source.z;
			mag = sqrt(
				reflected.x * reflected.x +
				reflected.y * reflected.y +
				reflected.z * reflected.z
				);
			reflected.x /= mag;
			reflected.y /= mag;
			reflected.z /= mag;

			draw_pixel(
				s, x, y, z,
				rgb(
					(uint8_t) (
						(start_color >> 24) + coef0 *
						(
							1. * (end_color >> 24) -
							(start_color >> 24)
							)
						),
					(uint8_t) (
						(start_color << 8 >> 24) + coef0 *
						(
							1. * (end_color << 8 >> 24) -
							(start_color << 8 >> 24)
							)
						),
					(uint8_t) (
						(start_color << 16 >> 24) + coef0 *
						(
							1. * (end_color << 16 >> 24) -
							(start_color << 16 >> 24)
							)
						)
					),
				start_shine + coef0 * (end_shine - start_shine),
				amb_color, light_color,
				point_to_source, normal, point_to_eye, reflected,
				amb_ref_constant, diff_ref_constant, spec_ref_constant
				);
		}
	}
}

char left_of(vertex in_question, vertex v0, vertex v1) {
	return (in_question.x - v0.x) * (v1.y - v0.y) -
	(in_question.y - v0.y) * (v1.x - v0.x) > 0;
}

void scanline_fill(
	screen s, vertex min, vertex mid, vertex max,
	uint32_t amb_color, uint32_t light_color, vector light_source, vector eye,
	double amb_ref_constant, double diff_ref_constant, double spec_ref_constant,
	char perspective
	) {
	int32_t start_y = (int32_t) min.y,
	end_y0 = (int32_t) mid.y, end_y1 = (int32_t) max.y;

	if (
		(mid.x - min.x) * (max.y - min.y) -
		(mid.y - min.y) * (max.x - min.x) < 0
		) {
		scanline_fill_half(
			s, start_y, end_y0, min, mid, min, max,
			amb_color, light_color, light_source, eye,
			amb_ref_constant, diff_ref_constant, spec_ref_constant, perspective
			);
		scanline_fill_half(
			s, end_y0, end_y1, mid, max, min, max,
			amb_color, light_color, light_source, eye,
			amb_ref_constant, diff_ref_constant, spec_ref_constant, perspective
			);
	}
	else {
		scanline_fill_half(
			s, start_y, end_y0, min, max, min, mid,
			amb_color, light_color, light_source, eye,
			amb_ref_constant, diff_ref_constant, spec_ref_constant, perspective
			);
		scanline_fill_half(
			s, end_y0, end_y1, min, max, mid, max,
			amb_color, light_color, light_source, eye,
			amb_ref_constant, diff_ref_constant, spec_ref_constant, perspective
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

vertex min_vertex(vertex v0, vertex v1, vertex v2) {
	vertex result = v0;
	if (v1.y < result.y) {
		result = v1;
	}
	if (v2.y < result.y) {
		result = v2;
	}
	return result;
}

vertex max_vertex(vertex v0, vertex v1, vertex v2) {
	vertex result = v0;
	if (v1.y > result.y) {
		result = v1;
	}
	if (v2.y > result.y) {
		result = v2;
	}
	return result;
}

vertex mid_vertex(vertex min, vertex max, vertex v0, vertex v1, vertex v2) {
	if (
		(v0.x != min.x || v0.y != min.y || v0.z != min.z) &&
		(v0.x != max.x || v0.y != max.y || v0.z != max.z)
		) {
		return v0;
	}
	if (
		(v1.x != min.x || v1.y != min.y || v1.z != min.z) &&
		(v1.x != max.x || v1.y != max.y || v1.z != max.z)
		) {
		return v1;
	}
	return v2;
}

void draw_faces(
	screen s, vertex_list vlist, face_list flist,
	vector center, vector eye, double distance,
	uint32_t amb_color, uint32_t light_color, vector light_source,
	double amb_ref_constant, double diff_ref_constant, double spec_ref_constant
	) {
	size_t num_faces = flist->size;
	face *faces = flist->list;
	matrix display = display_matrix(center, eye);
	vertex *vertices = transformed_vertices(vlist, display);
	light_source = transformed_vector(light_source, display);
	free(display);
	face f;
	vertex v0, v1, v2, min, max;
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
				min = min_vertex(v0, v1, v2);
				max = max_vertex(v0, v1, v2);

				scanline_fill(
					s, min, mid_vertex(min, max, v0, v1, v2), max,
					amb_color, light_color, light_source, eye,
					amb_ref_constant, diff_ref_constant, spec_ref_constant,
					PERSPECTIVE_ON
					);
			}
		}
	}
	else {
		while (num_faces--) {
			f = *faces++;
			v0 = vertices[f.v0]; v1 = vertices[f.v1]; v2 = vertices[f.v2];
			if (is_front_face(v0, v1, v2)) {
				min = min_vertex(v0, v1, v2);
				max = max_vertex(v0, v1, v2);
				scanline_fill(
					s, min, mid_vertex(min, max, v0, v1, v2), max,
					amb_color, light_color, light_source, eye,
					amb_ref_constant, diff_ref_constant, spec_ref_constant,
					PERSPECTIVE_OFF
					);
			}
		}
	}
	free(vertices);
}

void add_box(
	vertex_list vlist, edge_list elist, face_list flist,
	double x, double y, double z,
	double w, double h, double d,
	vertex text0, vertex text1
	) {
	size_t first_index = vlist->size;

	double x2 = x + w, y2 = y - h, z2 = z - d;

	add_vertex(vlist, x , y , z ,  0,  0,  1, text0, text1);
	add_vertex(vlist, x , y2, z ,  0,  0,  1, text0, text1);
	add_vertex(vlist, x2, y2, z ,  0,  0,  1, text0, text1);
	add_vertex(vlist, x2, y , z ,  0,  0,  1, text0, text1);

	add_vertex(vlist, x2, y , z ,  1,  0,  0, text0, text1);
	add_vertex(vlist, x2, y2, z ,  1,  0,  0, text0, text1);
	add_vertex(vlist, x2, y2, z2,  1,  0,  0, text0, text1);
	add_vertex(vlist, x2, y , z2,  1,  0,  0, text0, text1);

	add_vertex(vlist, x2, y , z2,  0,  0, -1, text0, text1);
	add_vertex(vlist, x2, y2, z2,  0,  0, -1, text0, text1);
	add_vertex(vlist, x , y2, z2,  0,  0, -1, text0, text1);
	add_vertex(vlist, x , y , z2,  0,  0, -1, text0, text1);

	add_vertex(vlist, x , y , z2, -1,  0,  0, text0, text1);
	add_vertex(vlist, x , y2, z2, -1,  0,  0, text0, text1);
	add_vertex(vlist, x , y2, z , -1,  0,  0, text0, text1);
	add_vertex(vlist, x , y , z , -1,  0,  0, text0, text1);

	add_vertex(vlist, x , y , z ,  0,  1,  0, text0, text1);
	add_vertex(vlist, x2, y , z ,  0,  1,  0, text0, text1);
	add_vertex(vlist, x2, y , z2,  0,  1,  0, text0, text1);
	add_vertex(vlist, x , y , z2,  0,  1,  0, text0, text1);

	add_vertex(vlist, x , y2, z2,  0, -1,  0, text0, text1);
	add_vertex(vlist, x2, y2, z2,  0, -1,  0, text0, text1);
	add_vertex(vlist, x2, y2, z ,  0, -1,  0, text0, text1);
	add_vertex(vlist, x , y2, z ,  0, -1,  0, text0, text1);

	add_edge(elist, first_index     , first_index +  1);
	add_edge(elist, first_index +  1, first_index +  2);
	add_edge(elist, first_index +  2, first_index +  3);
	add_edge(elist, first_index +  3, first_index     );

	add_edge(elist, first_index +  4, first_index +  5);
	add_edge(elist, first_index +  5, first_index +  6);
	add_edge(elist, first_index +  6, first_index +  7);
	add_edge(elist, first_index +  7, first_index +  4);

	add_edge(elist, first_index +  8, first_index +  9);
	add_edge(elist, first_index +  9, first_index + 10);
	add_edge(elist, first_index + 10, first_index + 11);
	add_edge(elist, first_index + 11, first_index +  8);

	add_edge(elist, first_index + 12, first_index + 13);
	add_edge(elist, first_index + 13, first_index + 14);
	add_edge(elist, first_index + 14, first_index + 15);
	add_edge(elist, first_index + 15, first_index + 12);

	add_edge(elist, first_index + 16, first_index + 17);
	add_edge(elist, first_index + 17, first_index + 18);
	add_edge(elist, first_index + 18, first_index + 19);
	add_edge(elist, first_index + 19, first_index + 16);

	add_edge(elist, first_index + 20, first_index + 21);
	add_edge(elist, first_index + 21, first_index + 22);
	add_edge(elist, first_index + 22, first_index + 23);
	add_edge(elist, first_index + 23, first_index + 20);

	add_face(flist, first_index     , first_index +  1, first_index +  2);
	add_face(flist, first_index +  2, first_index +  3, first_index     );

	add_face(flist, first_index +  4, first_index +  5, first_index +  6);
	add_face(flist, first_index +  6, first_index +  7, first_index +  4);

	add_face(flist, first_index +  8, first_index +  9, first_index + 10);
	add_face(flist, first_index + 10, first_index + 11, first_index +  8);

	add_face(flist, first_index + 12, first_index + 13, first_index + 14);
	add_face(flist, first_index + 14, first_index + 15, first_index + 12);

	add_face(flist, first_index + 16, first_index + 17, first_index + 18);
	add_face(flist, first_index + 18, first_index + 19, first_index + 16);

	add_face(flist, first_index + 20, first_index + 21, first_index + 22);
	add_face(flist, first_index + 22, first_index + 23, first_index + 20);
}

void add_sphere(
	vertex_list vlist, edge_list elist, face_list flist,
	double cx, double cy, double cz,
	double r, int steps, vertex text0, vertex text1
	) {
	size_t first_index = vlist->size;
	add_vertex(vlist, cx, cy, cz + r, 0, 0, 1, text0, text1);

	int step_count, half_step_count,
	half_steps = steps / 2;
	double phi = 0.0, phi_inc = M_PI / half_steps,
	theta, theta_inc = 2 * M_PI / steps,
	sin_phi, cos_phi, sin_theta, cos_theta, pre_xy, z;
	for (half_step_count = 1; half_step_count < half_steps; half_step_count++) {
		phi += phi_inc;
		sin_phi = sin(phi);
		cos_phi = cos(phi);
		pre_xy = r * sin_phi;
		z = cz + r * cos_phi;
		add_vertex(
			vlist, cx + pre_xy, cy, z, sin_phi, 0, cos_phi, text0, text1
			);
		theta = 0;
		for (step_count = 1; step_count < steps; step_count++) {
			theta += theta_inc;
			sin_theta = sin(theta);
			cos_theta = cos(theta);
			add_vertex(
				vlist,
				cx + pre_xy * cos_theta, cy + pre_xy * sin_theta, z,
				sin_phi * cos_theta, sin_phi * sin_theta, cos_phi, text0, text1
				);
		}
	}

	add_vertex(vlist, cx, cy, cz - r, 0, 0, -1, text0, text1);

	for (step_count = 1; step_count < steps; step_count++) {
		add_edge(elist, first_index, first_index + step_count);
		add_face(
			flist,
			first_index, first_index + step_count, first_index + step_count + 1
			);
	}
	add_edge(elist, first_index, first_index + step_count);
	add_face(flist, first_index, first_index + step_count, first_index + 1);

	half_steps -= 2;
	for (half_step_count = 0; half_step_count < half_steps; half_step_count++) {
		for (step_count = 1; step_count < steps; step_count++) {
			add_edge(
				elist,
				first_index + half_step_count * steps + step_count,
				first_index + half_step_count * steps + steps + step_count
				);
			add_edge(
				elist,
				first_index + half_step_count * steps + step_count,
				first_index + half_step_count * steps + step_count + 1
				);
			add_edge(
				elist,
				first_index + half_step_count * steps + step_count,
				first_index + half_step_count * steps + steps + step_count + 1
				);
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
		add_edge(
			elist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + step_count
			);
		add_edge(
			elist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + 1
			);
		add_edge(
			elist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + 1
			);
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
		add_edge(
			elist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + 1
			);
		add_edge(
			elist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + step_count + 1
			);
		add_face(
			flist,
			first_index + half_step_count * steps + step_count,
			first_index + half_step_count * steps + steps + 1,
			first_index + half_step_count * steps + step_count + 1
			);
	}
	add_edge(
		elist,
		first_index + half_step_count * steps + step_count,
		first_index + half_step_count * steps + steps + 1
		);
	add_edge(
		elist,
		first_index + half_step_count * steps + step_count,
		first_index + half_step_count * steps + 1
		);
	add_face(
		flist,
		first_index + half_step_count * steps + step_count,
		first_index + half_step_count * steps + steps + 1,
		first_index + half_step_count * steps + 1
		);
}

void draw_line(
	screen s, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color
	) {
	int32_t dx = abs(x1 - x0);
	int32_t dy = abs(y1 - y0);

	int32_t x_step = x0 < x1 ? 1 : -1;
	int32_t y_step = y0 < y1 ? 1 : -1;

	int32_t error = (dx > dy ? dx : -dy) / 2;
	int32_t original_error;
	while (1) {
		plot(s, x0, y0, 1, color);
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

void draw_edges(
	screen s, vertex_list vlist, edge_list elist,
	vector center, vector eye, double distance, uint32_t color
	) {
	size_t num_edges = elist->size;
	edge *edges = elist->list;
	matrix display = display_matrix(center, eye);
	vertex *vertices = transformed_vertices(vlist, display);
	free(display);
	edge e;
	vertex v0, v1;
	double factor;
	if (distance) {
		distance *= -1;
		while (num_edges--) {
			e = *edges++;
			v0 = vertices[e.v0]; v1 = vertices[e.v1];
			v0.x *= distance / v0.z; v0.y *= distance / v0.z;
			v1.x *= distance / v1.z; v1.y *= distance / v1.z;
			if (v0.z < 0 && v1.z < 0) {
				draw_line(s, v0.x, v0.y, v1.x, v1.y, color);
			}
			else if (v0.z >= 0 && v1.z >= 0) {
				continue;
			}
			else if (v0.z >= 0) {
				factor = -v1.z / (v0.z - v1.z);
				draw_line(
					s,
					factor * v0.x + (1 - factor) * v1.x,
					factor * v0.y + (1 - factor) * v1.y,
					v1.x, v1.y,
					color
					);
			}

			else {
				factor = -v0.z / (v1.z - v0.z);
				draw_line(
					s,
					v0.x, v0.y,
					(1 - factor) * v0.x + factor * v1.x,
					(1 - factor) * v0.y + factor * v1.y,
					color
					);
			}
		}
	}
	else {
		while (num_edges--) {
			e = *edges++;
			v0 = vertices[e.v0]; v1 = vertices[e.v1];
			draw_line(s, v0.x, v0.y, v1.x, v1.y, color);
		}
	}
	free(vertices);
}

void add_line(
	vertex_list vlist, edge_list elist,
	double x0, double y0, double z0, double x1, double y1, double z1,
	vertex text0, vertex text1
	) {
	size_t first_index = elist->size;

	add_vertex(vlist, x0, y0, z0, 0, 0, 1, text0, text1);
	add_vertex(vlist, x1, y1, z1, 0, 0, 1, text0, text1);

	add_edge(elist, first_index, first_index + 1);
}

/*
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
