#ifndef PNG_H
#define PNG_H
#include "screen.h"

#if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
	#define OS_WINDOWS
#endif

#define PNG_RGB 2 // code used in PNG format to denote "truecolor"
#define PNG_RGBA 6 // code used in PNG format to denote "truecolor with alpha"

void make_png(const char *name, const screen s, char color_type);
void display_png(const screen s, char color_type);

#endif
