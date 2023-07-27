#include "maindef.h"

color_t defcol_white = { 1.0f, 1.0f, 1.0f, 1.0f };
color_t defcol_black = { 0.0f, 0.0f, 0.0f, 1.0f };
color_t defcol_red = { 1.0f, 0.0f, 0.0f, 1.0f };

void Color_make(color_t col, float r, float g, float b, float a) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
	col[3] = a;
}
