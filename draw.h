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

typedef struct edge_struct {
	size_t v0;
	size_t v1;
} edge;

typedef struct edge_list_struct {
	size_t size;
	size_t capacity;
	edge *list;
} *edge_list;

#define PERSPECTIVE_ON 1;
#define PERSPECTIVE_OFF 0;

vertex_list new_vlist(size_t max_num_vertices);

void free_vlist(vertex_list vlist);

void clear_vlist(vertex_list vlist);

void transform(vertex_list vlist, matrix m);

face_list new_flist(size_t max_num_faces);

void free_flist(face_list flist);

void clear_flist(face_list flist);

edge_list new_elist(size_t max_num_edges);

void free_elist(edge_list elist);

void clear_elist(edge_list elist);

void draw_faces(
	screen s, vertex_list vlist, face_list flist,
	vector center, vector eye, double distance
	);

void add_box(
	vertex_list vlist, face_list flist,
	double x, double y, double z,
	double w, double h, double d,
	uint32_t color
	);

void add_sphere(
	vertex_list vlist, face_list flist,
	double cx, double cy, double cz,
	double r, int steps, uint32_t color
	);

void draw_edges(
	screen s, vertex_list vlist, edge_list elist,
	vector center, vector eye, double distance
	);

void add_line(
	vertex_list vlist, edge_list elist,
	double x0, double y0, double z0, double x1, double y1, double z1,
	uint32_t color
	);
#endif
