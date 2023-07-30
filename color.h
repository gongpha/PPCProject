#ifndef _H_COLOR_H_
#define _H_COLOR_H_

#include "maindef.h"

typedef float color_t[4];
typedef struct {
	uint8_t r; uint8_t g; uint8_t b;
} rgb_t;

void Color_make(color_t col, float r, float g, float b, float a);
void Color_copy(color_t src, color_t dest);

color_t defcol_white;
color_t defcol_black;
color_t defcol_red;

color_t defcol_gray;

#endif