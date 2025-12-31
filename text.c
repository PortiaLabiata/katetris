#include "text.h"
#include "font.h"
#include "tetris.h"

void suppx_text_draw(vbuf_t *vbuf, int x, int y) {
	vbuf_draw_rect(vbuf, x*GRID_STEP/2, y*GRID_STEP/2, 
					GRID_STEP/2, GRID_STEP/2);
}

static void _draw_char(vbuf_t *vbuf, char_t ptr, int x, int y) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (ptr[i] & (1 << j)) {
				suppx_text_draw(vbuf, x+i, y+j);
			}
		}
	}
}

void text_draw_char(vbuf_t *vbuf, char c, int x, int y) {
	if (x % GRID_STEP != 0 || y % GRID_STEP != 0) 
		return;
	for (int i = 0; i < FONT_MAP_SIZE; i++) {
		if (c == font_map[i].c) {
			_draw_char(vbuf, font_map[i].ptr, x, y);				
		}
	}
}

void text_draw_number(vbuf_t *vbuf, uint8_t v, int x, int y) {
	if (v == 0) {
		text_draw_char(vbuf, 0, x, y);
		return;
	}

	while (v > 0) {
		text_draw_char(vbuf, v % 10, x, y);
		v /= 10;
	}
}
