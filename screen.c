#include "screen.h"

#include <stdio.h>

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
	return r << 16 | g << 8 | b;
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

void plot(screen s, int32_t x, int32_t y, uint32_t value) {
	if (x >= 0 && x < s->width && y >= 0 && y < s->height) {
		s->data[s->width * (s->height - y - 1) + x] = value;
	}
}
