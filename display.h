#pragma once
#include "env.h"
#include "impl.h"

#include <inttypes.h>

void vbuf_init(vbuf_t *buf);
void vbuf_set_bit(vbuf_t *vbuf, int col, int row);
void vbuf_reset_bit(vbuf_t *vbuf, int col, int row);
void vbuf_clear(vbuf_t *buf);
void vbuf_set_flag(vbuf_t *vbuf);
void vbuf_draw_rect(vbuf_t *buf, int x, int y, int h, int w);

bool bbox_stack_push(bbox_stack_t *stack, bbox_t *box);
bool bbox_stack_pop(bbox_stack_t *stack, bbox_t *box);
