#ifndef PNG_H
#define PNG_H
#include <stdlib.h>

#define PNG_RGB 2 // code used in PNG format to denote "truecolor"
#define PNG_RGBA 6 // code used in PNG format to denote "truecolor with alpha"

void make_png(
	const char *name, size_t width, size_t height,
	const void *data, char color_type
	);

#endif
