#ifndef DRAW_H
#define DRAW_H
#include "coords.h"
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

#define PERSPECTIVE_ON 1;
#define PERSPECTIVE_OFF 0;

vertex_list new_vlist(size_t max_num_vertices);

void free_vlist(vertex_list vlist);

void clear_vlist(vertex_list vlist);

void transform(vertex_list vlist, matrix m);

face_list new_flist(size_t max_num_faces);

void free_flist(face_list flist);

void clear_flist(face_list flist);

void draw_polygons(
	screen s, vertex_list vlist, face_list flist, matrix perp, char perspective
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
#endif
