#include "screen.h"

#include <stdio.h>
#include <stdlib.h>

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
	return r << 24 | g << 16 | b << 8;
}

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return r << 24 | g << 16 | b << 8 | a;
}

screen make_screen(size_t width, size_t height) {
	if ((uint64_t)width * height > SIZE_MAX) {
		fprintf(stderr, "Screen error: number of pixels exceeds SIZE_MAX\n");
		exit(EXIT_FAILURE);
	}
	screen s = (screen) malloc(sizeof(struct screen_struct));
	if (s == NULL) {
		perror("Screen error (malloc)");
		exit(EXIT_FAILURE);
	}
	s->width = width;
	s->height = height;
	s->data = (uint32_t *) calloc(width, height * sizeof(uint32_t));
	if (s->data == NULL) {
		perror("Screen error (calloc)");
		exit(EXIT_FAILURE);
	}
	return s;
}

void free_screen(screen s) {
	free(s->data);
	free(s);
}

void plot(screen s, int32_t x, int32_t y, uint32_t color) {
	int32_t width = (int32_t) s->width, height = (int32_t) s->height;
	if (
		x > -width / 2 && x <= width / 2 && y > -height / 2 && y <= height / 2
		) {
		s->data[
			width * (height - y - (height - 1) / 2 - 1) + x + (width - 1) / 2
			] = color;
	}
}
