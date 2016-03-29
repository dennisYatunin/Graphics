#ifndef SCREEN_H
#define SCREEN_H
#include "matrix.h"
#include <stdint.h>

typedef struct screen_struct {
	size_t width;
	size_t height;
	uint32_t *data;
} *screen;

screen make_screen(size_t width, size_t height);

void free_screen(screen s);

void plot(screen s, int32_t x, int32_t y, uint32_t value);

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif
