#ifndef SCREEN_H
#define SCREEN_H
#include <stddef.h>
#include <stdint.h>

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

typedef struct screen_struct {
	size_t width;
	size_t height;
	uint32_t *data;
	double *z_buf;
} *screen;

screen make_screen(size_t width, size_t height);

void free_screen(screen s);

void clear_screen(screen s);

void plot(screen s, int32_t x, int32_t y, double z, uint32_t color);

#endif
