#ifndef PNG_H
#define PNG_H
#include "screen.h"

#define PNG_RGB 2 // code used in PNG format to denote "truecolor"
#define PNG_RGBA 6 // code used in PNG format to denote "truecolor with alpha"

void make_png(const char *name, const screen s, char color_type);

#endif
