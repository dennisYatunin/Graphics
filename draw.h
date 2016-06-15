#ifndef DRAW_H
#define DRAW_H
#include "matrix.h"
#include "screen.h"

#define max(a, b) \
({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

#define min(a, b) \
({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

typedef struct vertex_struct {
	double x;
	double y;
	double z;
	double nx;
	double ny;
	double nz;
	double shine;
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

#define PERSPECTIVE_ON 1
#define PERSPECTIVE_OFF 0

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
	vector center, vector eye, double distance,
	uint32_t amb_color, uint32_t light_color, vector light_source,
	double amb_ref_constant, double diff_ref_constant, double spec_ref_constant
	);

void add_box(
	vertex_list vlist, edge_list elist, face_list flist,
	double x, double y, double z,
	double w, double h, double d,
	vertex text0, vertex text1
	);

void add_sphere(
	vertex_list vlist, edge_list elist, face_list flist,
	double cx, double cy, double cz,
	double r, int steps, vertex text0, vertex text1
	);

void draw_edges(
	screen s, vertex_list vlist, edge_list elist,
	vector center, vector eye, double distance, uint32_t color
	);

void add_line(
	vertex_list vlist, edge_list elist,
	double x0, double y0, double z0, double x1, double y1, double z1,
	vertex text0, vertex text1
	);
#endif
