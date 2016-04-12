#ifndef DRAW_H
#define DRAW_H
#include "matrix.h"
#include "screen.h"

typedef struct vertex_struct {
	double x;
	double y;
	double z;
	uint32_t color;
} vertex;

typedef struct vertex_list_struct {
	size_t size;
	size_t capacity;
	vertex *list;
} *vertex_list;

vertex_list new_vlist(size_t max_num_vertices);

void free_vlist(vertex_list vlist);

void clear_vlist(vertex_list vlist);

void add_vertex(
	vertex_list vlist, double x, double y, double z, uint32_t color
	);

void transform(vertex_list vlist, matrix m);

typedef struct face_struct {
	size_t v0;
	size_t v1;
	size_t v2;
} face;

typedef struct face_list_struct {
	size_t size;
	size_t capacity;
	face *list;
} *face_list;

face_list new_flist(size_t max_num_faces);

void free_flist(face_list flist);

void clear_flist(face_list flist);

void add_face(face_list flist, size_t v0, size_t v1, size_t v2);

void draw_line(
	screen s,
	int32_t x0, int32_t y0, uint32_t color0,
	int32_t x1, int32_t y1, uint32_t color1
	);

typedef struct perspective_data_struct {
	uint8_t active;
	double view_x; double view_y; double view_z;
	double sx1; double sy1; double c1;
	double sx2; double sy2; double sz2; double c2;
	double sx3; double sy3; double sz3; double c3;
} *perspective_data;

perspective_data new_pdata();

void activate_perspective(perspective_data pdata);

void deactivate_perspective(perspective_data pdata);

void update_pdata(
	perspective_data pdata,
	double center_x, double center_y, double center_z,
	double camera_x, double camera_y, double camera_z,
	double distance
	);

void draw_edge(screen s, vertex v0, vertex v1, perspective_data pdata);

void draw_polygons(
	screen s, vertex_list vlist, face_list flist, perspective_data pdata
	);

void add_box(
	vertex_list vlist, face_list flist,
	double x, double y, double z,
	double w, double h, double d,
	uint32_t color
	);

/*
typedef struct point_matrix_struct {
	size_t size;
	matrix points;
	uint32_t *colors;
} *point_matrix;

point_matrix make_point_matrix(size_t capacity);

void clear_point_matrix(point_matrix pm);

void draw_line(
	screen s,
	int32_t x0, int32_t y0,
	int32_t x1, int32_t y1,
	uint32_t color
	);

void draw_lines(point_matrix pm, screen s);

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

void add_box(
	point_matrix pm,
	double x, double y, double z,
	double w, double h, double d,
	uint32_t color
	);

void add_sphere(
	point_matrix pm,
	double cx, double cy, double cz,
	double r, int steps, uint32_t color
	);

void add_torus(
	point_matrix pm,
	double cx, double cy, double cz,
	double r1, double r2,
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
*/
#endif
