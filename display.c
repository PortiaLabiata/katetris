#include "display.h"

#if !SITL
#include "chmtx.h"
#include "chprintf.h"
#endif

#include <string.h>

void vbuf_init(vbuf_t *buf) {
	thd->mtx_init(&buf->mtx);
	for (int i = 0; i < DISP_COLS; i++) {
		for (int j = 0; j < DISP_ROWS; j++) {
			buf->buf[i][j] = 0xFF;
		}
	}
}

void vbuf_set_bit(vbuf_t *vbuf, int col, int row) {
	vbuf->buf[col][row/8] |= 1 << (row % 8);
}

void vbuf_reset_bit(vbuf_t *vbuf, int col, int row) {
	vbuf->buf[col][row/8] &= ~(1 << (row % 8));
}

void vbuf_clear(vbuf_t *vbuf) {
	thd->lock(&vbuf->mtx);
	for (int i = 0; i < DISP_COLS; i++) {
		memset(vbuf->buf[i], 0x00, DISP_ROWS);
	}
	thd->unlock(&vbuf->mtx);
}

void vbuf_draw_rect(vbuf_t *vbuf, int x, int y, int h, int w) {
	thd->lock(&vbuf->mtx);
	for (int i = x; i < DISP_COLS && i < x+w; i++) {
		for (int j = y; j < DISP_ROWS*8 && j < y+h; j++) {
			vbuf_set_bit(vbuf, i, j);
		}
	}
	thd->unlock(&vbuf->mtx);
}

void vbuf_set_flag(vbuf_t *vbuf) {
	thd->lock(&vbuf->mtx);
	vbuf->clear_flag = true;
	thd->unlock(&vbuf->mtx);
}

bool bbox_stack_push(bbox_stack_t *stack, bbox_t *box) {
	if (stack->size < STACKSIZE) {
		stack->arr[stack->size++] = *box;
		return true;
	}
	return false;
}

bool bbox_stack_pop(bbox_stack_t *stack, bbox_t *box) {
	if (stack->size) {
		*box = stack->arr[--stack->size];
		return true;
	}
	return false;
}
