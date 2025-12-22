#pragma once
#include "env.h"
#include "impl.h"

#include <inttypes.h>

void vbuf_init(vbuf_t *buf);
void vbuf_set_bit(vbuf_t *vbuf, int col, int row);
void vbuf_reset_bit(vbuf_t *vbuf, int col, int row);
void vbuf_clear(vbuf_t *buf);
void vbuf_draw_rect(vbuf_t *buf, int x, int y, int h, int w);
