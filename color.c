#include "maindef.h"

color_t defcol_white = { 1.0f, 1.0f, 1.0f, 1.0f };
color_t defcol_black = { 0.0f, 0.0f, 0.0f, 1.0f };
color_t defcol_red = { 1.0f, 0.0f, 0.0f, 1.0f };

color_t defcol_gray = { 0.5f, 0.5f, 0.5f, 1.0f };

void Color_make(color_t col, float r, float g, float b, float a) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
	col[3] = a;
}

void Color_copy(color_t src, color_t dest) {
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

